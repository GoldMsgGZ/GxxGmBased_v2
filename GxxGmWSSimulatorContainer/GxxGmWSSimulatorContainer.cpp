// GxxGmWSSimulatorContainer.cpp : 定义控制台应用程序的入口点。
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

#include "../GxxGmWSSimulator/GxxGmWSSimulator.h"

#pragma comment(lib, "GxxGmWSSimulator.lib")

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
		//loadConfiguration();

		// 初始化日志
		std::string name = "log_";
		name.append(Poco::DateTimeFormatter::format(Poco::Timestamp(), "%Y%m%d%H%M%S"));
		name.append(".log");

		Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel);
		fileChannel->setProperty("path", name);
		fileChannel->setProperty("rotation", "1 M");
		fileChannel->setProperty("archive", "timestamp");

		// 每条日志的时间格式
		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());
		patternFormatter->setProperty("pattern","%Y %m %d %H:%M:%S %s(%l): %t");

		//初始化　Channel
		Poco::AutoPtr<Poco::Channel> channel = new Poco::FormattingChannel(patternFormatter,fileChannel);
		logger().setChannel(channel);
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
			// 读取采集站配置文件
			// 首先获得当前工作目录
			std::string current_working_dir = Poco::Path::current();
			Poco::Path config_path(current_working_dir);
			config_path.append("GxxGmWSSimulaterConf.ini");
			this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

			// 取得需要模拟的设备个数
			int simulater_count = config().getInt("WorkStationSimulater.INSTANCE_COUNT");

			// 国标ID前缀
			std::string gbcode_pre = config().getString("WorkStationSimulater.GBCODE_PRE");
			int gbcode_start = config().getInt("WorkStationSimulater.GBCODE_ID_START");

			// 设备IP
			std::string simulater_ip = config().getString("WorkStationSimulater.DEVICE_IP");

			// 几个自动操作的频率
			int heartbeat_rate = config().getInt("WorkStationSimulater.HEARTBEAT_RATE");
			int fileupload_rate = config().getInt("WorkStationSimulater.FILE_UPLOAD_RATE");

			// 基本媒体文件路径
			std::string video_name = config().getString("WorkStationSimulater.VIDEO_FILE");
			std::string audio_name = config().getString("WorkStationSimulater.AUDIO_FILE");
			std::string image_name = config().getString("WorkStationSimulater.IMAGE_FILE");

			// 获取接入网关配置
			std::string gateway_ip = config().getString("DeviceGateway.SERVER_IP");
			int gateway_port = config().getInt("DeviceGateway.SERVER_PORT");
			std::string gateway_domain = config().getString("DeviceGateway.SERVER_DOMAIN");
			std::string gateway_authkey = config().getString("DeviceGateway.SERVER_AUTHKEY");

			// 文件上传相关参数
			std::string file_domain = config().getString("FILE_UPOLAD.DOMAIN");		// 文件所属域
			std::string dsj_id = config().getString("DSJ_INFO.DSJ_GBCODE");			// 执法仪国标ID
			//std::string police_name = config().getString("FILE_UPOLAD.POLICE_NAME");		// 民警姓名
			int file_size = config().getInt("FILE_UPOLAD.FILE_SIZE");					// 文件大小
			std::string file_type = config().getString("FILE_UPOLAD.FILE_TYPE");			// 文件类型
			int file_duration = config().getInt("FILE_UPOLAD.FILE_DURATION");				// 文件时长
			std::string tag_info = config().getString("FILE_UPOLAD.TAG_INFO");			// 备注信息
			std::string org_code = config().getString("FILE_UPOLAD.ORG_CODE");			// 单位编号或部门编号
			std::string org_name = config().getString("FILE_UPOLAD.ORG_NAME");			// 单位名称
			std::string police_code = config().getString("FILE_UPOLAD.POLICE_CODE");		// 警员编号
			std::string police_name = config().getString("FILE_UPOLAD.POLICE_NAME");		// 警员姓名
			std::string storage_path = config().getString("FILE_UPOLAD.STORAGE_PATH");		// 存储位置
			std::string local_path = config().getString("FILE_UPOLAD.LOCAL_PATH");		// 物理位置，采集站上原文件本机存储路径
			std::string play_path = config().getString("FILE_UPOLAD.PLAY_PATH");			// 播放位置：HTTP访问路径
			std::string storage_server = config().getString("FILE_UPOLAD.STORAGE_SERVER");	// 存储服务器
			std::string thumb = config().getString("FILE_UPOLAD.THUMB");

			// 然后循环初始化指定数量的工作站模拟器
			Poco::Logger &logger = this->logger();
			std::vector<GxxGmWSSimulator *> simulators;
			for (int index = 0; index < simulater_count; ++index)
			{
				// 当前模拟器的ID
				char current_device_id[4096] = {0};
				sprintf_s(current_device_id, 4096, "%s%07d", gbcode_pre.c_str(), gbcode_start + index);

				// 当前模拟器文件全路径
				Poco::Path video_path(current_working_dir);
				video_path.append(video_name);
				std::string video_path_str = video_path.toString(Poco::Path::PATH_NATIVE);

				Poco::Path audio_path(current_working_dir);
				audio_path.append(video_name);
				std::string audio_path_str = audio_path.toString(Poco::Path::PATH_NATIVE);

				Poco::Path image_path(current_working_dir);
				image_path.append(image_name);
				std::string image_path_str = image_path.toString(Poco::Path::PATH_NATIVE);

				GxxGmWSSimulator *simulater = new GxxGmWSSimulator;
				simulater->SetLogger(&logger);

				errCode = simulater->Initialize(current_device_id, simulater_ip.c_str(), heartbeat_rate,
					fileupload_rate, video_path_str.c_str(), audio_path_str.c_str(), image_path_str.c_str(),
					gateway_ip.c_str(),	gateway_port, gateway_authkey.c_str(), gateway_domain.c_str());

				if (errCode != 0)
				{
					// 有模拟器初始化失败了
				}

				simulater->SetFileUploadInfo(file_domain, dsj_id, file_size, file_type, file_duration, 
					tag_info, org_code, org_name, police_code, police_name, storage_path, local_path,
					play_path, storage_server, thumb);
			}


			// 等待终端退出请求
			std::cout<<"Press \"Enter\" key to exit ...";
			getchar();

			// 
			std::vector<GxxGmWSSimulator *>::iterator iter;
			for (iter = simulators.begin(); iter != simulators.end();)
			{
				GxxGmWSSimulator *simulater = *iter;
				simulater->Close();

				delete simulater;
				iter = simulators.erase(iter);
			}
		}
		catch (Poco::Exception e)
		{
			errCode = e.code();
			errStr = e.displayText();
		}

		return Poco::Util::Application::EXIT_OK;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	printf("高新兴国迈 采集工作站模拟器 V1.1\n");
	printf("\n");
	printf("功能说明：\n");
	printf("1. 支持接入3.0平台；\n");
	printf("2. 支持采集站上线，发送容量、CPU状态、内存状态等信息；\n");
	printf("3. 支持采集站上报文件信息，默认1秒发一次，配置文件可配频率；\n");
	//printf("4. 支持向接入平台发送扩展的设备定位信息，默认5秒发一次，配置文件可配频率；\n");
	//printf("5. 支持实时视频点流，流数据来源默认为video.gmf，配置文件可配其他格式视频文件，视频文件编码必须为H.264；\n");
	//printf("6. 修正无帧率视频的播放，默认按30pfs进行播放；\n");
	//printf("7. 当视频源为G711时，可推送音频数据；\n");
	//printf("8. 模拟器可配置定位坐标；\n");
	printf("\n");
	system("pause");

	GxxGmApplication app;
	app.run();

	return 0;
}

