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

int GxxGmGoVideo::GetDeviceStatus(const char *device_id)
{
	int errCode = 0;

	return errCode;
}

int GxxGmGoVideo::GetRealStream(const char *device_id)
{
	int errCode = 0;

	return errCode;
}