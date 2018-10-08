#ifndef _GxxGmHttpServer_H_
#define _GxxGmHttpServer_H_

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HttpServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h" 
#include "Poco/StreamCopier.h"

#include <iostream>
#include <string>
#include <vector>

// 请求处理类
class GxxGmRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

//
class GxxGmRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
};

class GxxGmHttpServer : public Poco::Util::ServerApplication
{
public:
	GxxGmHttpServer();
	~GxxGmHttpServer();

protected:
	int main(const std::vector<std::string>& args);

public:
	void Start();
	void Stop();

private:
	bool is_need_stop_;
	Poco::Net::HTTPServer *instance_;
};

#endif//_GxxGmHttpServer_H_
