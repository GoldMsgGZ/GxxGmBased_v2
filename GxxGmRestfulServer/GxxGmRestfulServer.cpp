// GxxGmRestfulServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HttpServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/StreamCopier.h"

#include "GxxGmRestfulPlugin.h"

class GxxGmRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		int errCode = 0;
		std::string errStr = "";
		try
		{
			// 这里会接收到外部发来的请求
			std::string uri = request.getURI();
			//std::string host = request.getHost();
			std::string method = request.getMethod();

			// 读取发过来的数据
			// 读取数据
			std::istream &is = request.stream();

			std::ostringstream ostr;
			std::istream *ptr_rs = &is;
			Poco::StreamCopier::copyStream(*ptr_rs, ostr);

			std::string request_body = ostr.str();

			std::cout<<"收到请求："<<std::endl;
			std::cout<<method.c_str()<<" "/*<<host.c_str()*/<<uri.c_str()<<std::endl;

			if (request_body.size() > 0)
				std::cout<<request_body.c_str()<<std::endl;

			std::cout<<std::endl;

			// 响应信息
			response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
			std::ostream &out = response.send();
		}
		catch(Poco::Exception &ex)
		{
			errCode = ex.code();
			errStr = ex.displayText();

			std::cout<<"接收请求时遇到异常，错误码："<<errCode<<"。错误信息："<<errStr.c_str()<<std::endl;
		}
	}

public:
	void SetPlugin(GxxGmRestfulPlugin *plugin)
	{
		plugins_.push_back(plugin);
	}

private:
	std::vector<GxxGmRestfulPlugin *> plugins_;
};

class GxxGmRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		return new GxxGmRequestHandler;
	}
};

class GxxGmServerApp : public Poco::Util::ServerApplication
{
protected:
	int main(const std::vector<std::string>& args)
	{
		int errCode = Poco::Util::Application::EXIT_OK;
		std::string errstr;

		try
		{
			// 首先获得当前工作目录
			std::string current_working_dir = Poco::Path::current();
			Poco::Path config_path(current_working_dir);
			config_path.append("config.ini");
			this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

			// 拿IP、端口进行绑定
			std::string ipaddress = config().getString("SERVICE_CFG.IP");
			unsigned int port = config().getUInt("SERVICE_CFG.PORT");
			Poco::Net::SocketAddress socket_address(ipaddress, port);
			Poco::Net::ServerSocket server_socket(socket_address);

			// 创建工厂类，加载插件对象给工厂类
			Poco::SharedPtr<GxxGmRequestHandlerFactory> factory = new GxxGmRequestHandlerFactory;

			Poco::Net::HTTPServer server(factory, server_socket, new Poco::Net::HTTPServerParams);
			server.start();
			waitForTerminationRequest();
			server.stop();
		}
		catch (Poco::Exception e)
		{
			errCode = e.code();
			errstr = e.displayText();
		}

		return Poco::Util::Application::EXIT_OK;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	GxxGmServerApp app;
	return app.run(argc, argv);
}

