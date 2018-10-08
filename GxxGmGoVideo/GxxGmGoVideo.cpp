#include "GxxGmGoVideo.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/PrintHandler.h"

#ifdef USE_HTTPSERVER
#include "GxxGmHttpServer.h"
#endif


GxxGmGoVideo::GxxGmGoVideo()
: http_session_(NULL)
{
	//
}

GxxGmGoVideo::~GxxGmGoVideo()
{
	//
	if (http_session_)
	{
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;
		delete session;
	}
	http_session_ = NULL;
}

int GxxGmGoVideo::Initialize(int http_port /* = 9900 */)
{
	// 启动一个线程，单独运行HTTP SERVER
	if (!http_server_thread_.isRunning())
	{
		http_server_thread_.start(GxxGmGoVideo::HttpServerThread, this);
	}

	return 0;
}

int GxxGmGoVideo::Login(const char *govideo_ip, unsigned short govideo_port, const char *sequence_id /* = "admin" */, const char *login_id /* = "admin" */, int login_type /* = 107 */, const char *username /* = "admin" */, const char *password /* = "admin" */)
{
	int errCode = 0;

	try {
		host_ = govideo_ip;
		port_ = govideo_port;
		http_session_ = new Poco::Net::HTTPClientSession(host_, port_);
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/LoginRequest?SequenceID=%s&LoginID=%s&LoginType=%d&UserName=%s&Password=%s&LoginIP=10.10.2.102",
			sequence_id, login_id, login_type, username, password);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// 分析结果
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		Poco::Dynamic::Var token = jsonObject->get("Token");

		token_ = token.toString();
		errCode = atoi(result_code.toString().c_str());

		// 登录成功了，启动心跳线程
	}
	catch (Poco::Net::NetException &ex)
	{
		std::string errstr = ex.displayText();
		printf("登录到GoVideo失败！%s\n", errstr.c_str());
	}

	return errCode;
}

int GxxGmGoVideo::Logout()
{
	int errCode = 0;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/LogoutRequest?SequenceID=1&Token=%s&LoginIP=192.168.30.17",
			token_);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// 分析结果
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
	}
	catch (Poco::Net::NetException &ex)
	{
		std::string errstr = ex.displayText();
		printf("从GoVideo注销登录失败！%s\n", errstr.c_str());
	}

	return 0;
}

int GxxGmGoVideo::GetAllDevices()
{
	int errCode = 0;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		// 这里只做了从索引0开始查询500个，当设备接入更多的时候我们这里是需要轮询查多次的，直到返回的结果中为0的时候，我们停止查询
		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/ServerConfig/http://GoVideo/Serviceconfig/GetAllDeviceRequest?SequenceID=cyfiddev111&StartRow=0&DevCount=300&Token=%s&lag=0",
			token_.c_str());
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// 判断服务器返回信息
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// 将字符串转为UTF-8
		wchar_t *tmp_buffer = new wchar_t[json_str.size() + 1024];
		MultiByteToWideChar(CP_ACP, 0, json_str.c_str(), -1, tmp_buffer, json_str.size() + 1024);
		char *json_str_utf8 = new char[json_str.size() + 1024];
		WideCharToMultiByte(CP_UTF8, 0, tmp_buffer, -1, json_str_utf8, json_str.size() + 1024, NULL, NULL);
		
		delete tmp_buffer;
		tmp_buffer = NULL;

		json_str = json_str_utf8;

		delete json_str_utf8;
		json_str_utf8 = NULL;

		// 分析结果
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());

		Poco::Dynamic::Var result_count = jsonObject->get("DevTotal");
		if (atoi(result_count.toString().c_str()) > 0)
		{
			Poco::JSON::Array::Ptr result_device_list = jsonObject->getArray("DeviceInfoList");
			Poco::JSON::Array::ConstIterator iter = result_device_list->begin();
			for (; iter != result_device_list->end(); ++iter)
			{
				Poco::JSON::Object::Ptr device_info_json_object = iter->extract<Poco::JSON::Object::Ptr>();

				Poco::Dynamic::Var device_id				= device_info_json_object->get("DeviceID");
				Poco::Dynamic::Var device_index				= device_info_json_object->get("DevIndex");
				Poco::Dynamic::Var device_name				= device_info_json_object->get("DeviceName");
				Poco::Dynamic::Var model_id					= device_info_json_object->get("ModelID");
				Poco::Dynamic::Var category_id				= device_info_json_object->get("CategoryID");
				Poco::Dynamic::Var device_code				= device_info_json_object->get("DevCode");
				Poco::Dynamic::Var device_connection_info	= device_info_json_object->get("DevConnectionInfo");
				Poco::Dynamic::Var device_version			= device_info_json_object->get("DevVersion");
				Poco::Dynamic::Var device_username			= device_info_json_object->get("DevUserName");
				Poco::Dynamic::Var device_password			= device_info_json_object->get("DevPassword");
				Poco::Dynamic::Var device_extra_info		= device_info_json_object->get("DevExInfo");
				Poco::Dynamic::Var gb28181_code				= device_info_json_object->get("GB28181Code");
				Poco::Dynamic::Var device_name_abbr			= device_info_json_object->get("DevNameAbbr");
				Poco::Dynamic::Var version					= device_info_json_object->get("Version");
				Poco::Dynamic::Var remark					= device_info_json_object->get("Remark");
				Poco::Dynamic::Var dgw_server_id			= device_info_json_object->get("DGWServerID");

				GOVIDEO_DEVICE_INFO *device_info = new GOVIDEO_DEVICE_INFO;

				device_info->device_id_				= atoi(device_id.toString().c_str());
				device_info->device_index_			= atoi(device_index.toString().c_str());
				device_info->device_name_			= device_name.toString();
				device_info->model_id_				= atoi(model_id.toString().c_str());
				device_info->category_id_			= atoi(category_id.toString().c_str());
				device_info->device_code_			= device_code.toString();
				device_info->device_connection_info_	= device_connection_info.toString();
				device_info->device_version_			= device_version.toString();
				device_info->device_username_		= device_username.toString();
				device_info->device_password_		= device_password.toString();
				device_info->device_extra_info_		= device_extra_info.toString();
				device_info->gb28181_code_			= gb28181_code.toString();
				device_info->device_name_abbr_		= device_name_abbr.toString();
				device_info->version_				= atoi(version.toString().c_str());
				device_info->remark_					= remark.toString();
				device_info->dgw_server_id_			= atoi(dgw_server_id.toString().c_str());

				//devices_.insert(std::pair<unsigned int, GOVIDEO_DEVICE_INFO*>(device_info->device_id_, device_info));
				devices_.push_back(device_info);
			}
		}
		
		
	}
	catch (Poco::Net::NetException &ex)
	{
		std::string errstr = ex.displayText();
		printf("从GoVideo获取所有设备信息失败！%s\n", errstr.c_str());
	}
	catch (Poco::JSON::JSONException &ex)
	{
		std::string errstr = ex.displayText();
		printf("从GoVideo获取所有设备信息失败！%s\n", errstr.c_str());
	}

	return errCode;
}

int GxxGmGoVideo::GetDeviceStatus(const char *device_gb28181_code, GOVIDEO_DEVICE_STATUS &device_status)
{
	int errCode = 0;

	// 首先在已经查询到的设备列表里面读取GoVideo注册的设备id
	unsigned int govideo_device_id = 0;
	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = devices_.begin(); iter != devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *dev_info = *iter;
		if (dev_info->gb28181_code_.compare(device_gb28181_code) == 0)
		{
			govideo_device_id = dev_info->device_id_;
			device_status.gb28181_code_ = device_gb28181_code;
			break;
		}
	}

	if (iter == devices_.end())
	{
		// 没找到设备
		return -1;
	}

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Device/GetDeviceStatusRequest?SequenceID=5&DeviceID=%d&Token=%s",
			govideo_device_id, token_.c_str());
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// 分析结果
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		device_status.device_id_ = govideo_device_id;

		Poco::Dynamic::Var dev_status = jsonObject->get("DevStatus");
		device_status.status_ = atoi(dev_status.toString().c_str());

		Poco::Dynamic::Var channel_count = jsonObject->get("ChannelNum");
		if (atoi(channel_count.toString().c_str()) > 0)
		{
			Poco::JSON::Array::Ptr channel_status_list = jsonObject->getArray("ChannelStatusList");
			Poco::JSON::Array::ConstIterator iter = channel_status_list->begin();
			for (; iter != channel_status_list->end(); ++iter)
			{
				Poco::JSON::Object::Ptr channel_status_json_object = iter->extract<Poco::JSON::Object::Ptr>();

				Poco::Dynamic::Var channel_index = channel_status_json_object->get("ChnIndex");
				Poco::Dynamic::Var channel_status = channel_status_json_object->get("Status");

				int chn_index = atoi(channel_index.toString().c_str());
				int chn_status = atoi(channel_status.toString().c_str());

				device_status.chennal_status_.insert(std::pair<int, unsigned int>(chn_index, chn_status));
			}
		}
		
	}
	catch (Poco::Net::NetException &ex)
	{
		std::string errstr = ex.displayText();
		printf("从GoVideo注销登录失败！%s\n", errstr.c_str());
	}

	return 0;
}

int GxxGmGoVideo::GetAllDeviceStatus()
{
	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = devices_.begin(); iter != devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *dev_info = *iter;
		GOVIDEO_DEVICE_STATUS device_status_;
		GetDeviceStatus(dev_info->gb28181_code_.c_str(), device_status_);

		devices_status_.push_back(device_status_);
	}

	return 0;
}

int GxxGmGoVideo::GetRealStream(unsigned int device_id, std::string &stream_url)
{
	int errCode = 0;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\"Message\":{\"DeviceID\":\"%d\",\"ClientType\":\"107\",\"ChannelNum\":1,\"StreamType\":1,\"IPAddr\":\"10.10.16.59\"}}",
			device_id);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Device/GetStreamRequest"
			);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);
		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request)<< http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (status != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
		{
			return status;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// 分析结果
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		Poco::Dynamic::Var stream_uri = jsonObject->get("StreamURI");
		stream_url = stream_uri.toString();
	}
	catch (Poco::Net::NetException &ex)
	{
		std::string errstr = ex.displayText();
		printf("点流失败！%s\n", errstr.c_str());
	}

	return errCode;
}

int GxxGmGoVideo::GetRealStreamByGBCode(const char *device_gb28181_code, std::string &stream_url)
{
	int errCode = 0;

	std::vector<GOVIDEO_DEVICE_STATUS>::iterator iter;
	for (iter = devices_status_.begin(); iter != devices_status_.end(); ++iter)
	{
		GOVIDEO_DEVICE_STATUS *dev_status = &(*iter);
		if (dev_status->gb28181_code_.compare(device_gb28181_code) == 0)
		{
			if (dev_status->status_ == 1)
			{
				// 设备不在线
				errCode = -1;
				break;
			}

			errCode = GetRealStream(dev_status->device_id_, stream_url);
			break;
		}
	}

	return errCode;
}


void GxxGmGoVideo::HttpServerThread(void* param)
{
#ifdef USE_HTTPSERVER
	int errCode = 0;
	GxxGmGoVideo *govideo = (GxxGmGoVideo *)param;

	govideo->http_server_ = new GxxGmHttpServer();

	std::vector<std::string> params;
	params.push_back("9900");
	govideo->http_server_->run(params);
	//govideo->http_server_->Start();

	while (true)
	{
		// 等待退出标记

		Sleep(1000);
	}
#endif
}

void GxxGmGoVideo::HeartBeatThread(void* param)
{
	int errCode = 0;
	GxxGmGoVideo *govideo = (GxxGmGoVideo *)param;

	while (true)
	{
		try {
			Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)govideo->http_session_;

			char query_string[4096] = {0};
			sprintf_s(query_string, 4096,
				"/GoVideo/Server/KeepAlive"
				);
			std::string uri = query_string;
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);
			request.add("Content-Type", "application/json; charset=gb2312");
			request.add("SequenceID", "10086");
			request.add("Token", govideo->token_.c_str());

			request.setContentLength(0);

			session->sendRequest(request);

			Poco::Net::HTTPResponse response;
			std::istream &is = session->receiveResponse(response);

			Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
			if (status != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
			{
				Sleep(5000);
				continue;
			}

			std::ostringstream ostr;
			Poco::StreamCopier::copyStream(is, ostr);

			std::string json_str = ostr.str();

			// 分析结果
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var json = parser.parse(json_str);
			Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

			Poco::Dynamic::Var message = jsonObject->get("Message");
			jsonObject = message.extract<Poco::JSON::Object::Ptr>();
			Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
			errCode = atoi(result_code.toString().c_str());

			// 5秒发一次心跳
			Sleep(5000);
		}
		catch (Poco::Net::NetException &ex)
		{
			std::string errstr = ex.displayText();
			printf("点流失败！%s\n", errstr.c_str());
		}
	}

}