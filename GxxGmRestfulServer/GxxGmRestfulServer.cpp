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

#include "Poco\ClassLoader.h"
#include "Poco\Manifest.h"
#include "GxxGmAbstractPlugin.h"

typedef Poco::ClassLoader<GxxGmAbstractPlugin> PluginLoader;
typedef Poco::Manifest<GxxGmAbstractPlugin> PluginManifest;

class GxxGmRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		// 有新的请求进来了，我们根据插件集合来分发
		PluginLoader::Iterator iter(plugin_loader_->begin());
		PluginLoader::Iterator iter_end(plugin_loader_->end());

		for (; iter != iter_end; ++iter)
		{
			PluginManifest::Iterator manifest_iter(iter->second->begin());
			PluginManifest::Iterator manifest_iter_end(iter->second->end());

			for (; manifest_iter != manifest_iter_end; ++manifest_iter)
			{
				int errCode = manifest_iter->
				if (errCode != REQUEST_HANDLE_CODE_NOT_SUPPORTED)
				{
					// 说明该请求已经被某一个业务插件处理掉了，这里的结果是最终结果
					break;
				}
			}
		}
	}

public:
	void SetLoader(PluginLoader *loader)
	{
		plugin_loader_ = loader;
	}

private:
	PluginLoader *plugin_loader_;
};

class GxxGmRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		GxxGmRequestHandler *handler = new GxxGmRequestHandler;
		handler->SetLoader(&plugin_loader_);
	}

public:
	void LoadPlugin(std::string plugin_path)
	{
		plugin_loader_.loadLibrary(plugin_path);
	}

private:
	PluginLoader plugin_loader_;
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

			// 读取插件配置
			int plugin_count = config().getInt("PLUGINS.PLUGIN_COUNT");

			for (int index = 0; index < plugin_count; ++index)
			{
				char value[4096] = {0};
				sprintf_s(value, 4096, "PLUGINS.PLUGIN_PATH_%d", index + 1);
				std::string plugin_path = config().getString(value);
				plugin_path += Poco::SharedLibrary::suffix();

				factory->LoadPlugin(plugin_path);
			}

			// 启动服务
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

