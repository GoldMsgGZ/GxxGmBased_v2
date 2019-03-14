#include "GxxGmPoliceStatuAndCasePlugin.h"

// 这个宏必须在cpp文件里面实现，否则没有清单信息
POCO_BEGIN_MANIFEST(AbstractPlugin)
POCO_EXPORT_CLASS(GxxGmPoliceStatuAndCasePlugin)
POCO_END_MANIFEST

std::string GxxGmPoliceStatuAndCasePlugin::name() const
{
	return "GxxGmPoliceStatuAndCasePlugin";
}

void GxxGmPoliceStatuAndCasePlugin::start(Poco::Util::Application *app)
{
	// 这里应该启动一个线程来处理此相关业务
	if (!working_thread_.isRunning())
	{
		app_ = app;
		working_thread_.start(GxxGmPoliceStatuAndCasePlugin::WorkingThread, this);
	}
	
	std::cout << "GxxGmPoliceStatuAndCasePlugin::start()" << std::endl;
}

void GxxGmPoliceStatuAndCasePlugin::WorkingThread(void* param)
{
	GxxGmPoliceStatuAndCasePlugin *plugin = (GxxGmPoliceStatuAndCasePlugin *)param;

	plugin->app_->logger().debug("GxxGmPoliceStatuAndCasePlugin::WorkingThread() started...");

	// 从配置文件中读取对应的信息，比如
	// 取得需要模拟的设备个数
	std::string gateway_ip = plugin->app_->config().getString("GATEWAY_INFO.IPADDRESS");
	int gateway_port = plugin->app_->config().getInt("GATEWAY_INFO.PORT");

	// 首先连接到对接服务器
	Poco::Net::SocketAddress address(gateway_ip, gateway_port);
	Poco::Net::StreamSocket socket(address);
	Poco::Net::HTTPSession session(socket);

	while (try)
	{
		// 组织
	}

	plugin->app_->logger().debug("GxxGmPoliceStatuAndCasePlugin::WorkingThread() stopped...");
	return ;
}

// optional set up and clean up functions
void pocoInitializeLibrary()
{
	std::cout << "PluginLibrary initializing" << std::endl;
}

void pocoUninitializeLibrary()
{
	std::cout << "PluginLibrary uninitializing" << std::endl;
}