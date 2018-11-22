// GxxGmLocalService.cpp : 定义控制台应用程序的入口点。
// https://blog.csdn.net/fengyishang/article/details/45220823

#include "stdafx.h"
#include <iostream>

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HttpServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h" 
#include "Poco/StreamCopier.h"

using Poco::Net::DNS;
using Poco::Net::IPAddress;
using Poco::Net::HostEntry;

class GxxGmRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	GxxGmRequestHandlerFactory();
	~GxxGmRequestHandlerFactory();

public:
	virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		return new RootHandler;
	}
};


int _tmain(int argc, _TCHAR* argv[])
{
	Poco::UInt16 port = 8901;
	Poco::Net::HTTPServerParams *params = new Poco::Net::HTTPServerParams;
	params->setMaxQueued(100);
	params->setMaxThreads(16);

	Poco::Net::ServerSocket server_socket(port);
	return 0;
}

