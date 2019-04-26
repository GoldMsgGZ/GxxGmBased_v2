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
#include "..\GxxGmDSJSimulater\FFMpegStub.h"
#include "..\bgBase\bgBase.h"

#pragma comment(lib, "bgBase.lib")

/**
 * �����Լ���Ӧ����
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

		std::string current_working_dir = Poco::Path::current();
		Poco::Path config_path(current_working_dir);
		config_path.append("GxxGmDSJSimulater.ini");
		this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

		int log_level = config().getInt("LOG_INFO.LEVEL");

		// ��ʼ����־
		std::string name = "DSJSimulator_log_";
		name.append(Poco::DateTimeFormatter::format(Poco::Timestamp(), "%Y%m%d%H%M%S"));
		name.append(".log");

		Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel);
		fileChannel->setProperty("path", name);
		fileChannel->setProperty("rotation", "50 M");
		fileChannel->setProperty("archive", "timestamp");

		// ÿ����־��ʱ���ʽ
		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());
		patternFormatter->setProperty("pattern","[%Y-%m-%d %H:%M:%S] %p %s(%l): %t");

		//��ʼ����Channel
		Poco::AutoPtr<Poco::Channel> channel = new Poco::FormattingChannel(patternFormatter,fileChannel);
		logger().setChannel(channel);
		logger().setLevel(log_level);
	}

	void uninitialize()
	{
		// ����ִ����ط���ʼ��
		Poco::Util::Application::uninitialize();
	}

	bool has_param(const char *param)
	{
		Poco::Util::Application::ArgVec argcs = this->argv();
		Poco::Util::Application::ArgVec::iterator iter;
		bool founded = false;
		for (iter = argcs.begin(); iter != argcs.end(); ++iter)
		{
			if (iter->compare("-q") == 0)
			{
				founded = true;
				break;
			}
		}

		return founded;
	}

	int main(const std::vector<std::string>& args)
	{
		int errCode = 0;
		std::string errStr;
		char msg[4096] = {0};

		printf("�����˹��� ִ����ģ����(GB28181-2016) V2.10\n");
		printf("\n");
		printf("����˵����\n");
		printf("1. ֧��GB28181-2011 �� GB28181-2016�����汾��Э��ͨ�ţ�\n");
		printf("2. ֧�������ƽ̨����GB28181����������Ĭ��30�뷢һ�Σ������ļ�����Ƶ�ʣ�\n");
		printf("3. ֧�������ƽ̨������չ���豸������Ϣ��Ĭ��5�뷢һ�Σ������ļ�����Ƶ�ʣ�\n");
		printf("4. ֧�������ƽ̨������չ���豸��λ��Ϣ��Ĭ��5�뷢һ�Σ������ļ�����Ƶ�ʣ�\n");
		printf("5. ֧��ʵʱ��Ƶ��������������ԴĬ��Ϊvideo.gmf�������ļ�����������ʽ��Ƶ�ļ�����Ƶ�ļ��������ΪH.264��\n");
		printf("6. ������֡����Ƶ�Ĳ��ţ�Ĭ�ϰ�30pfs���в��ţ�\n");
		printf("7. ����ƵԴΪG711ʱ����������Ƶ���ݣ�\n");
		printf("8. ģ���������ö�λ���ꣻ\n");
		printf("9. ģ����28181�����������豸������Ϣ���豸��λ��Ϣ���ͼ��֧�ֵ����뼶��\n");
		printf("10. ����Poco::Application���ʵ��ģ������\n");
		printf("11. �Ż��˷��������������Դռ�ã�\n");
		printf("12. ��������Ƶ���������ù��̣���һ��������Դռ�ã�\n");
		printf("13. �ع���־ϵͳ��\n");
		printf("14. �������˻����߼��������˽��վ����߼���\n");
		printf("15. ������������-q������ȷ�ϻ���ֱ������ģ������\n");
		printf("\n");

		if (!has_param("-q"))
			system("pause");

		try
		{
			// ��ȡ�ɼ�վ�����ļ�
			// ���Ȼ�õ�ǰ����Ŀ¼
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
			int dev_userbind_time = config().getUInt("GxxGmDSJSimulater.DEV_BINDUSER_TIME");
			int dev_imei_index_start = config().getUInt("GxxGmDSJSimulater.DEVICE_IMEI_START");

			std::string police_number = config().getString("GxxGmDSJSimulater.POLICE_NUMBER");
			std::string police_password = config().getString("GxxGmDSJSimulater.POLICE_PASSWORD");

			std::string location_latitude = config().getString("GxxGmDSJSimulater.DEV_LOCATION_LATITUDE");
			std::string location_longtitude = config().getString("GxxGmDSJSimulater.DEV_LOCATION_LONGTITUDE");

			// ׼��FFMpegStub
			char current_program_path[4096] = {0};
			GetModuleFileNameA(NULL, current_program_path, 4096);
			std::string tmp = current_program_path;
			int pos = tmp.find_last_of('\\');

			// �����ʼ��FFMpeg
			std::string ffmpeg_base_dir = tmp.substr(0, pos + 1);
			
			FFMpegStub *ffmpeg_stub = new FFMpegStub;
			errCode = ffmpeg_stub->Initialize(ffmpeg_base_dir.c_str());

			std::vector<GxxGmDSJSimulater *> simulaters;

			// �����豸�������������豸
			for (int index = 0; index < simulater_count; ++index)
			{
				// ����ģ�����˿ں�
				int current_client_port = local_port_start + index;
				char current_client_port_string[64] = {0};
				sprintf_s(current_client_port_string, 64, "%d", current_client_port);

				// ����ģ�����������
				char current_client_device_index[8] = {0};
				sprintf_s(current_client_device_index, "%07d", local_gbcode_id_start + index);

				char current_client_gbcode[21] = {0};
				sprintf_s(current_client_gbcode, 21, "%s%s", local_gbcode_pre.c_str(), current_client_device_index);

				// ����ģ����IMEI
				int current_client_imei_index = dev_imei_index_start + index;
				char current_client_imei[64] = {0};
				sprintf_s(current_client_imei, 64, "GxxGmSim%04d", current_client_imei_index);

				// ����RTP��صĶ˿���Ϣ
				int current_client_rtp_port_begin = rtp_port_begin * (1 + index) + rtp_port_count * index + 1;
				int current_client_rtp_port_end = current_client_rtp_port_begin + rtp_port_count - 1;

				GxxGmDSJSimulater *simulater = new GxxGmDSJSimulater();

				// �����豸������Ϣ
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

				// �����豸��λ��Ϣ
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
				init_info.dev_userbind_time_ = dev_userbind_time;
				init_info.police_number_ = police_number;
				init_info.police_password_ = police_password;
				init_info.imei_ = current_client_imei;
				init_info.platform_id_ = platform_id;

				int errCode = simulater->Initialize(init_info, ffmpeg_stub, this);
				if (errCode != 0)
				{
					sprintf_s(msg, 4096, "��ʼ��%dģ����%sʧ�ܣ������룺%d", index, current_client_gbcode, errCode);
					std::cout<<msg<<std::endl;
					logger().error(msg);
					
					delete simulater;
					continue;
				}

				simulaters.push_back(simulater);
			}

			printf("ģ�����Ѿ����У���������ر�ģ����....");
			logger().information("ģ�����Ѿ����У���������ر�ģ����....");
			getchar();

			// ��������ע��
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


int main(int argc, const char *argv[])
{
	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string tmp = current_program_path;
	int pos = tmp.find_last_of('\\');

	// ��ʼ������ҵ��Ŀ¼

	// �����ʼ��FFMpeg
	std::string base_dir = tmp.substr(0, pos + 1);

	Poco::DateTime current_datetime;
	char dump_path[4096] = {0};
	sprintf_s(dump_path, 4096, "%s%d%02d%02d_%02d%02d%02d.dmp", base_dir.c_str(),
		current_datetime.year(), current_datetime.month(), current_datetime.day(),
		current_datetime.hour(), current_datetime.minute(), current_datetime.second());

	bgBase::SetupMiniDumpMonitor(dump_path);

	GxxGmApplication app;
	app.init(argc, (char **)argv);
	app.run();

	return 0;
}