// GxxGmPoliceStatuAndCaseSimulator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include "Poco/DateTimeFormatter.h"
#include "Poco/FileChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Timestamp.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#include "Poco\ClassLoader.h"
#include "Poco\Manifest.h"
#include "..\GxxGmPoliceStatuAndCasePlugin\AbstractPlugin.h"

typedef Poco::ClassLoader<AbstractPlugin> PluginLoader;
typedef Poco::Manifest<AbstractPlugin> PluginManifest;

/**
 * 定义自己的应用类
 */
class GxxGmApplication : public Poco::Util::Application
{
public:
	GxxGmApplication() {}
	~GxxGmApplication() {}

public:
	void initialize(Poco::Util::Application& self)
	{
		Poco::Util::Application::initialize(self);

		// 这里执行相关初始化，例如在这里加载指定的配置文件？
		// 读取采集站配置文件
		// 首先获得当前工作目录
		std::string current_working_dir = Poco::Path::current();
		Poco::Path config_path(current_working_dir);
		config_path.append("GxxGmPoliceStatuAndCaseSimulatorConf.ini");
		this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

		// 取日志级别
		int log_level = config().getInt("LOG_INFO.LEVEL");

		// 初始化日志
		std::string name = "log_GxxGmPoliceStatuAndCaseSimulator_";
		name.append(Poco::DateTimeFormatter::format(Poco::Timestamp(), "%Y%m%d%H%M%S"));
		name.append(".log");

		Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel);
		fileChannel->setProperty("path", name);
		fileChannel->setProperty("rotation", "100 M");
		fileChannel->setProperty("archive", "timestamp");
		

		// 每条日志的时间格式
		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());
		patternFormatter->setProperty("pattern","[%p][%Y-%m-%d %H:%M:%S]: %t");
		patternFormatter->setProperty("times", "local");

		//初始化　Channel
		Poco::AutoPtr<Poco::Channel> channel = new Poco::FormattingChannel(patternFormatter,fileChannel);
		logger().setChannel(channel);
		logger().setLevel(log_level);
	}

	void uninitialize()
	{
		// 这里执行相关反初始化
		Poco::Util::Application::uninitialize();
	}

	int main(const std::vector<std::string>& args)
	{
		int errCode = 0;
		std::string errStr;

		try
		{
			
			// 加载插件，进行业务处理
			PluginLoader loader;
			std::string libName("GxxGmPoliceStatuAndCasePlugin");
			libName += Poco::SharedLibrary::suffix(); // append .dll or .so
			loader.loadLibrary(libName);

			AbstractPlugin* pPluginA = loader.create("GxxGmPoliceStatuAndCasePlugin");
			std::cout << pPluginA->name() << std::endl;

			pPluginA->start(this);

			getchar();
		}
		catch (Poco::Exception e)
		{
			errCode = e.code();
			errStr = e.displayText();

			std::cout<<"Exception >>> "<<errStr.c_str()<<" error code :"<<errCode<<std::endl;
		}

		return Poco::Util::Application::EXIT_OK;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	GxxGmApplication app;
	int errCode = app.run();

	return errCode;
}

