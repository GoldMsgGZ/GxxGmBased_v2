#include <iostream>
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

#include "..\GxxGmDSJSimulater\GxxGmDSJSimulater.h"

//#include "CrashRpt.h"
//#pragma comment(lib, "CrashRpt1403.lib")
//#pragma comment(lib, "CrashRptProbe1403.lib")

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

		// 初始化日志
		std::string name = "DSJSimulator_log_";
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
			config_path.append("GxxGmDSJSimulater.ini");
			this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

			std::string server_ip = config().getString("DeviceGateway.SERVER_IP");
			std::string server_port = config().getString("DeviceGateway.SERVER_PORT");
			std::string server_gbcode = config().getString("DeviceGateway.SERVER_GBCODE");
			std::string server_username = config().getString("DeviceGateway.SERVER_USERNAME");
			std::string server_password = config().getString("DeviceGateway.SERVER_PASSWORD");

			int simulater_count = config().getUInt("GxxGmDSJSimulater.INSTANCE_COUNT");
			std::string local_ip = config().getString("GxxGmDSJSimulater.DEVICE_IP");;
			int local_port_start = config().getUInt("GxxGmDSJSimulater.DEVICE_PORT_START");
			std::string local_gbcode_pre = config().getString("GxxGmDSJSimulater.GBCODE_PRE");
			int local_gbcode_id_start = config().getUInt("GxxGmDSJSimulater.GBCODE_ID_START");

			int manual_port = config().getUInt("GxxGmDSJSimulater.MANUAL_PORT");
			int rtp_port_begin = config().getUInt("GxxGmDSJSimulater.RTP_PORT_START");
			int rtp_port_count = config().getUInt("GxxGmDSJSimulater.RTP_PORT_COUNT");

			std::string sip_net = config().getString("GxxGmDSJSimulater.SIP_NET");
			std::string rtp_net = config().getString("GxxGmDSJSimulater.RTP_NET");
			std::string stream_file = config().getString("GxxGmDSJSimulater.SIM_STREAM_FILE");
			std::string platform_id = config().getString("GxxGmDSJSimulater.PLATFORM_ID");

			int gb28181_hb_time = config().getUInt("GxxGmDSJSimulater.GB28181_HB_TIME");
			int dev_baseinfo_time = config().getUInt("GxxGmDSJSimulater.DEV_BASE_INFO_TIME");
			int dev_location_time = config().getUInt("GxxGmDSJSimulater.DEV_LOCATION_TIME");
			int dev_imei_index_start = config().getUInt("GxxGmDSJSimulater.DEVICE_IMEI_START");

			std::string location_latitude = config().getString("GxxGmDSJSimulater.DEV_LOCATION_LATITUDE");
			std::string location_longtitude = config().getString("GxxGmDSJSimulater.DEV_LOCATION_LONGTITUDE");

			std::vector<GxxGmDSJSimulater *> simulaters;

			// 根据设备数量启动所有设备
			for (int index = 0; index < simulater_count; ++index)
			{
				// 构建模拟器端口号
				int current_client_port = local_port_start + index;
				char current_client_port_string[64] = {0};
				sprintf_s(current_client_port_string, 64, "%d", current_client_port);

				// 构建模拟器国标编码
				char current_client_device_index[8] = {0};
				sprintf_s(current_client_device_index, "%07d", local_gbcode_id_start + index);

				char current_client_gbcode[21] = {0};
				sprintf_s(current_client_gbcode, 21, "%s%s", local_gbcode_pre.c_str(), current_client_device_index);

				// 构建模拟器IMEI
				int current_client_imei_index = dev_imei_index_start + index;
				char current_client_imei[64] = {0};
				sprintf_s(current_client_imei, 64, "GxxGmSim%04d", current_client_imei_index);

				// 计算RTP相关的端口信息
				int current_client_rtp_port_begin = rtp_port_begin * (1 + index) + rtp_port_count * index + 1;
				int current_client_rtp_port_end = current_client_rtp_port_begin + rtp_port_count - 1;

				GxxGmDSJSimulater *simulater = new GxxGmDSJSimulater();

				// 设置设备基本信息
				DEVICE_BASE_INFO base_info;
				base_info.carrieroperator_ = BASE_INFO_Carrieroperator_CMCC;
				base_info.net_type_ = BASE_INFO_Nettype_4G;
				base_info.signal_ = "100";
				base_info.battery_ = "100";
				base_info.storage_ = "64";
				base_info.cpu_ = "30";
				base_info.version_ = "GxxGm_simulater_1.0.0.1";
				base_info.local_record_ = BASE_INFO_LocalRecord_NO;
				simulater->SetBaseInfo(base_info);

				// 设置设备定位信息
				DEVICE_LOCATION_INFO location_info;
				location_info.division_ns_ = LOCATION_INFO_DivisionN;
				location_info.division_ew_ = LOCATION_INFO_DivisionE;
				location_info.radius_ = "3";
				location_info.longitude_ = location_longtitude;
				location_info.latitude_ = location_latitude;
				location_info.direction_ = "0";
				location_info.speed_ = "0";
				location_info.height_ = "0";
				location_info.satellites_ = "20";
				location_info.location_available_ = "1";
				simulater->SetLocationInfo(location_info);

				struct SimulaterInitInfo init_info;
				init_info.local_ip_ = local_ip;
				init_info.local_port_ = current_client_port_string;
				init_info.local_gbcode_ = current_client_gbcode;
				init_info.server_ip_ = server_ip;
				init_info.server_port_ = server_port;
				init_info.server_gbcode_ = server_gbcode;
				init_info.username_ = server_username;
				init_info.password_ = server_password;
				init_info.manual_port_ = manual_port;
				init_info.begin_port_ = current_client_rtp_port_begin;
				init_info.end_port_ = current_client_rtp_port_end;
				init_info.sip_net_ = sip_net;
				init_info.rtp_net_ = rtp_net;
				init_info.stream_file_ = stream_file;
				init_info.gb28181_hb_time_ = gb28181_hb_time;
				init_info.dev_baseinfo_time_ = dev_baseinfo_time;
				init_info.dev_location_time_ = dev_location_time;
				init_info.imei_ = current_client_imei;
				init_info.platform_id_ = platform_id;

				int errCode = simulater->Initialize(init_info);
				if (errCode != 0)
				{
					printf("初始化%d模拟器%s失败，错误码：%d\n", index, current_client_gbcode, errCode);
					delete simulater;
					continue;
				}

				simulaters.push_back(simulater);
			}

			printf("模拟器已经运行，按任意键关闭模拟器....");
			getchar();

			// 这里批量注销
			std::vector<GxxGmDSJSimulater *>::iterator iter;
			for (iter = simulaters.begin(); iter != simulaters.end(); ++iter)
			{
				(*iter)->Destroy();
				delete *iter;
			}

			simulaters.clear();
		}
		catch (Poco::Exception e)
		{
			errCode = e.code();
			errStr = e.displayText();
		}

		return Poco::Util::Application::EXIT_OK;
	}
};

BOOL WINAPI CrashCallback(LPVOID lpvState)
{
	UNREFERENCED_PARAMETER(lpvState);

	// Crash happened!
	MessageBoxA(NULL, "发生了崩溃！", "", 0);

	return TRUE;
}

int main(int argc, const char *argv[])
{
	printf("高新兴国迈 执法仪模拟器(GB28181-2016) V2.6\n");
	printf("\n");
	printf("功能说明：\n");
	printf("1. 支持GB28181-2011 和 GB28181-2016两个版本的协议通信；\n");
	printf("2. 支持向接入平台发送GB28181保活心跳，默认30秒发一次，配置文件可配频率；\n");
	printf("3. 支持向接入平台发送扩展的设备基础信息，默认5秒发一次，配置文件可配频率；\n");
	printf("4. 支持向接入平台发送扩展的设备定位信息，默认5秒发一次，配置文件可配频率；\n");
	printf("5. 支持实时视频点流，流数据来源默认为video.gmf，配置文件可配其他格式视频文件，视频文件编码必须为H.264；\n");
	printf("6. 修正无帧率视频的播放，默认按30pfs进行播放；\n");
	printf("7. 当视频源为G711时，可推送音频数据；\n");
	printf("8. 模拟器可配置定位坐标；\n");
	printf("9. 模拟器28181保活心跳、设备基础信息、设备定位信息发送间隔支持到毫秒级；\n");
	printf("10. 采用Poco::Application框架实现模拟器；\n");
	printf("\n");
	system("pause");

	//// 使用CrashRpt获取崩溃记录
	//CR_INSTALL_INFO info;
	//memset(&info, 0, sizeof(CR_INSTALL_INFO));
	//info.cb = sizeof(CR_INSTALL_INFO);             // Size of the structure
	//info.pszAppName = "GxxGmDSJSimulaterContainer"; // App name
	//info.pszAppVersion = "2.6";              // App version
	//info.pszEmailSubject = "GxxGmDSJSimulaterContainer (GB28181-2016) V2.6 Crash Report"; // Email subject
	//info.pszEmailTo = "wangyu@gosuncn.com";      // Email recipient address
	//info.pfnCrashCallback = CrashCallback;
	//info.pszErrorReportSaveDir = "D:\\";

	//info.dwFlags = 0;                    
	//info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;

	//// Install crash handlers
	//int nInstResult = crInstall(&info);            
	////assert(nInstResult==0);

	//// Check result
	//if(nInstResult!=0)
	//{
	//	char buff[256];
	//	crGetLastErrorMsg(buff, 256); // Get last error
	//	printf("%s\n", buff); // and output it to the screen
	//	return FALSE;
	//}

	GxxGmApplication app;
	app.run();

	//crUninstall();

	return 0;
}