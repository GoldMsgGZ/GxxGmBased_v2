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
{
	//
}

GxxGmGoVideo::~GxxGmGoVideo()
{
	//
}

int GxxGmGoVideo::Login(const char *govideo_ip, unsigned short govideo_port, const char *sequence_id /* = "admin" */, const char *login_id /* = "admin" */, int login_type /* = 107 */, const char *username /* = "admin" */, const char *password /* = "admin" */)
{
	int errCode = 0;

	try {
		host_ = govideo_ip;
		port_ = govideo_port;
		Poco::Net::HTTPClientSession session(host_, port_);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/LoginRequest?SequenceID=%s&LoginID=%s&LoginType=%d&UserName=%s&Password=%s&LoginIP=10.10.2.102",
			sequence_id, login_id, login_type, username, password);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session.sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session.receiveResponse(response);

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
		Poco::Net::HTTPClientSession session(host_, port_);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/LogoutRequest?SequenceID=1&Token=%s&LoginIP=192.168.30.17",
			token_);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session.sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session.receiveResponse(response);

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
		Poco::Net::HTTPClientSession session(host_, port_);

		// 这里只做了从索引0开始查询500个，当设备接入更多的时候我们这里是需要轮询查多次的，直到返回的结果中为0的时候，我们停止查询
		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/ServerConfig/http://GoVideo/Serviceconfig/GetAllDeviceRequest?SequenceID=cyfiddev111&StartRow=0&DevCount=500&Token=%s&lag=0",
			token_);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session.sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session.receiveResponse(response);

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

				GOVIDEO_DEVICE_INFO device_info;
			}
		}
		
		
	}
	catch (Poco::Net::NetException &ex)
	{
		std::string errstr = ex.displayText();
		printf("从GoVideo注销登录失败！%s\n", errstr.c_str());
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