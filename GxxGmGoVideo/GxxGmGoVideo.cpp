#include "GxxGmGoVideo.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/URI.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"

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