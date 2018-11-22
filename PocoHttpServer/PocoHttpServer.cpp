// PocoHttpServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/ServerApplication.h" 

#include <iostream>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Util::ServerApplication;
using Poco::Util::Application;


class RequestHandler : public HTTPRequestHandler
{
public:
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		response.setChunkedTransferEncoding(true);
		response.setContentType("text/html");

		std::ostream &ostr = response.send();
		ostr<<"<h1>Hello World!</h1>";
	}
};

class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
		return new RequestHandler();
	}
};

class MyApp : public ServerApplication
{
protected:
	int main(const std::vector<std::string>& args)
	{
		HTTPServer server(new RequestHandlerFactory, ServerSocket(9900), new HTTPServerParams);
		server.start();
		waitForTerminationRequest();
		server.stop();
		return Application::EXIT_OK;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	MyApp app;
	app.run(argc, argv);
	return 0;
}

