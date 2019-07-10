#include "GxxGmDSJSimulater.h"
#include <Windows.h>
#include <sstream>
#include <iostream>

#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/MD5Engine.h"
#include "Poco/Latin1Encoding.h"
#include "Poco/TextConverter.h"
#include "Poco/UTF8Encoding.h"
#include "Poco/Random.h"

#include "tinyxml2.h"

#define USE_REALSTREAM

#ifdef USE_FFMPEG
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
//#pragma comment(lib, "avformat.lib")
//#pragma comment(lib, "avformat.lib")
//#pragma comment(lib, "avformat.lib")
#endif

#ifdef _USE_SPEAKER_
#include "GxxGmWordSpeaker.h"
#pragma comment(lib, "GxxGmWordSpeaker.lib")
#endif

GxxGmDSJSimulater::GxxGmDSJSimulater()
: agent_(NULL)
, is_gb28181_heartbeat_thread_need_exit_(false)
, log_file_handle_(NULL)
, gb28181_hb_time_(30)
, dev_baseinfo_time_(5)
, dev_location_time_(5)
#ifdef _USE_SPEAKER_
, speaker_(new GxxGmWordSpeaker())
#endif
, notifer_(this)
, is_standard_gb28181_mobile_position_(false)
{
	// 
}

GxxGmDSJSimulater::~GxxGmDSJSimulater()
{
#ifdef _USE_SPEAKER_
	if (speaker_)
		delete speaker_;
#endif
}

int GxxGmDSJSimulater::Initialize(struct SimulaterInitInfo &init_info, FFMpegStub *ffmpeg_stub, Poco::Util::Application *app)
{
	int errCode = 0;
	char msg[4096] = {0};

	app_ = app;

#ifdef _USE_SPEAKER_
	errCode = speaker_->Initialize();
#endif

	// ��ʼ��Э��ջ
	//agent_ = GB28181Agent_Init(2, 16, Enum28181Version::eVERSION_2016, 3000);
	agent_ = GB28181Agent_Init(1, 1, Enum28181Version::eVERSION_2016, 3000);
	if (agent_ == NULL)
	{
		sprintf_s(msg, 4096, "[%s]��ʼ��28181Э��ջʧ�ܣ�", init_info.local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
		return -1;
	}

	// ��װ�ص�
	GB28181Agent_SetLogCB(agent_, _AgentLogCallBack, this);
	GB28181Agent_SetDevInfoQueryCB(agent_, _DevInfoQueryCB, this);
	GB28181Agent_SetDevControlCB(agent_, _DevControlCB, this);
	GB28181Agent_SetPlayControlCB(agent_, _PlayControlCB, this);
	GB28181Agent_SetStreamRequestCB(agent_, _StreamRequestCB, this);
	GB28181Agent_SetNotifyInfoCB(agent_, _NotifyInfo_CallBackFunc, this);
	GB28181Agent_SetExtendRequestCB(agent_, _ExtendRqeustCallBack, this);

	// �����ж��������
	EnumTransType eTransType = EnumTransType::eTYPE_UDP;
	if (init_info.sip_net_.compare("UDP") == 0)
		eTransType = EnumTransType::eTYPE_UDP;
	else if (init_info.sip_net_.compare("TCP") == 0)
		eTransType = EnumTransType::eTYPE_TCP;

	GS28181_ERR err = GB28181Agent_Start(agent_, init_info.local_ip_.c_str(), atoi(init_info.local_port_.c_str()), init_info.local_gbcode_.c_str(), eTransType);
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]����28181Э��ջʧ�ܣ������룺%d", init_info.local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);

		GB28181Agent_Uninit(agent_);
		return err;
	}

	reg_msg.iExpires = 86400;	// 1��
	strcpy_s(reg_msg.szUserName, STR_USERNAME_LEN, init_info.username_.c_str());
	strcpy_s(reg_msg.szPassword, STR_PASSWORD_LEN, init_info.password_.c_str());
	strcpy_s(reg_msg.stuCnnParam.szIP, STR_IPADDRESS_LEN, init_info.server_ip_.c_str());
	reg_msg.stuCnnParam.iPort = (unsigned int)atoi(init_info.server_port_.c_str());
	strcpy_s(reg_msg.stuCnnParam.szGBCode, STR_GBCODE_LEN, init_info.server_gbcode_.c_str());

	char date_time[4096] = {0};
	err = GB28181Agent_Register(agent_, &reg_msg, date_time);
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]ע�ᵽ��������ʧ�ܣ������룺%d", init_info.local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);

		GB28181Agent_Stop(agent_);
		GB28181Agent_Uninit(agent_);
		return err;
	}

	sprintf_s(msg, 4096, "[%s]ע�ᵽ�������سɹ��������룺%d", init_info.local_gbcode_.c_str(), err);
	std::cout<<msg<<std::endl;
	app_->logger().information(msg);

	// �������
	local_ip_ = init_info.local_ip_.c_str();
	local_port_ = init_info.local_port_.c_str();
	local_gbcode_ = init_info.local_gbcode_.c_str();
	server_ip_ = init_info.server_ip_.c_str();
	server_port_ = init_info.server_port_.c_str();
	server_gbcode_ = init_info.server_gbcode_.c_str();
	username_ = init_info.username_.c_str();
	password_ = init_info.password_.c_str();

	gb28181_hb_time_ = init_info.gb28181_hb_time_;
	dev_baseinfo_time_ = init_info.dev_baseinfo_time_;
	dev_location_time_ = init_info.dev_location_time_;
	dev_userbind_time_ = init_info.dev_userbind_time_;

	police_id_ = init_info.police_number_;
	police_password_ = init_info.police_password_;

	imei_ = init_info.imei_;
	platform_id_ = init_info.platform_id_;

	// ����Ĭ������
	dev_name_ = "GxxGmSimulater";
	frame_rate_ = "25";
	bit_rate_ = "600000";
	trans_resolution_ = "600000";

	machine_id_ = "000000";
	bind_user_id_ = "000000";
	bind_user_name_ = "";
	dep_name_ = "";
	dep_id_ = "000000";
	show_location_ = "YES";
	show_datetime_ = "YES";

	// ��ʼ��������ģ��
	stream_mgr_.SetFFmpegStub(ffmpeg_stub);
	errCode = stream_mgr_.Initialize(init_info.manual_port_, init_info.begin_port_, init_info.end_port_, init_info.local_ip_.c_str(), init_info.rtp_net_.c_str(), init_info.stream_file_.c_str());
	if (errCode != 0)
	{
		sprintf_s(msg, 4096, "[%s]��ʼ����������ʧ�ܣ�", init_info.local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]��ʼ����������ɹ���", init_info.local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	// ��ѯĿ¼��Ӧ�ɹ�
	// ��ʼ���������豸״̬��Ϣ�Ͷ�λ��Ϣ
	// ʹ��Poco�Ķ��߳̿������
	if (!gb28181_heartbeat_thread_.isRunning())
	{
		gb28181_heartbeat_thread_.start(GB28181HeartbeatThreadFun, this);
		//Sleep(10);
		Poco::Thread::sleep(10);
	}

	if (!extra_data_response_thread.isRunning())
	{
		extra_data_response_thread.start(ExtraDataResponseThread, this);
		Poco::Thread::sleep(10);
	}

	return errCode;
}

void GxxGmDSJSimulater::Destroy()
{
	// �ȷ������ֹͣ�������ͣ�����ֹͣ������Ϣ����λ��Ϣ������
	is_gb28181_heartbeat_thread_need_exit_ = true;
	gb28181_heartbeat_thread_.join();

	// ֹͣ����
	stream_mgr_.StopRealStream();

	// ��ע��
	StruRegistMsg reg_msg;

	reg_msg.iExpires = 0;	// 1��
	strcpy_s(reg_msg.szUserName, STR_USERNAME_LEN, username_.c_str());
	strcpy_s(reg_msg.szPassword, STR_PASSWORD_LEN, password_.c_str());
	strcpy_s(reg_msg.stuCnnParam.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	reg_msg.stuCnnParam.iPort = (unsigned int)atoi(server_port_.c_str());
	strcpy_s(reg_msg.stuCnnParam.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());

	char date_time[4096] = {0};
	GB28181Agent_Register(agent_, &reg_msg, date_time);

	GB28181Agent_Stop(agent_);
	GB28181Agent_Uninit(agent_);

	CloseHandle(log_file_handle_);
	log_file_handle_ = NULL;

	is_standard_gb28181_mobile_position_ = false;
}

void GxxGmDSJSimulater::SetNotifer(GxxGmDSJSimulaterNotifer *notifer)
{
	notifer_ = notifer;
}

void GxxGmDSJSimulater::SetBaseInfo(DEVICE_BASE_INFO base_info)
{
	// 
	base_info_.battery_			= base_info.battery_;
	base_info_.carrieroperator_	= base_info.carrieroperator_;
	base_info_.cpu_				= base_info.cpu_;
	base_info_.local_record_	= base_info.local_record_;
	base_info_.net_type_		= base_info.net_type_;
	base_info_.signal_			= base_info.signal_;
	base_info_.storage_			= base_info.storage_;
	base_info_.version_			= base_info.version_;
}

void GxxGmDSJSimulater::SetLocationInfo(DEVICE_LOCATION_INFO location_info)
{
	// 
	location_info_.direction_			= location_info.direction_;
	location_info_.division_ew_			= location_info.division_ew_;
	location_info_.division_ns_			= location_info.division_ns_;
	location_info_.height_				= location_info.height_;
	location_info_.latitude_			= location_info.latitude_;
	location_info_.location_available_	= location_info.location_available_;
	location_info_.location_time_		= location_info.location_time_;
	location_info_.longitude_			= location_info.longitude_;
	location_info_.radius_				= location_info.radius_;
	location_info_.satellites_			= location_info.satellites_;
	location_info_.speed_				= location_info.speed_;

	// ��ʼ��
	last_longtitude_ = atof(location_info_.longitude_.c_str());
	last_latitude_ = atof(location_info_.latitude_.c_str());
}

void GxxGmDSJSimulater::SetExceptionInfo(DEVICE_EXCEPTION_INFO excep_info)
{
	// 
	exception_info_.battery_	= excep_info.battery_;
	exception_info_.ccd_		= excep_info.ccd_;
	exception_info_.storage_	= excep_info.storage_;
	exception_info_.mic_		= excep_info.mic_;
}

int GxxGmDSJSimulater::SendBindUserInfo(/*const char *platform_id, const char *device_imei, */const char *user_id, const char *password)
{
	// ����Ҫ�󹹽��ַ���
	const char *msg_format = "<SubCmdType>BindUser</SubCmdType> \
							  <DeviceStates> \
									<PlatformID>%s</PlatformID> \
									<DeviceIMEI>%s</DeviceIMEI> \
									<UserID>%s</UserID> \
									<Password>%s</Password> \
							  </DeviceStates>";

	// ����һ�¾���������
	police_id_ = user_id;
	police_password_ = password;

	// ������һ��MD5
	Poco::MD5Engine md5;
	md5.update(password);
	std::string md5hex = md5.digestToHex(md5.digest());
	//std::string md5hex = password;

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, 
		platform_id_.c_str(), imei_.c_str(), user_id, md5hex.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]�����˻�������ʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]�����˻�������ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendBindUserConfirmRecevicedInfo()
{
	// ������������У����߳�ר�Ŵ�����Ϣ����
	GS28181_ERR err = GS28181_ERR_SUCCESS;

	char msg[4096] = {0};
	strcpy_s(msg, 4096, "<SubCmdType>BindUserConfirmReceived</SubCmdType>");

	ExtraDataResponseInfo info;
	info.extra_msg_ = msg;
	extra_response_queue_.push(info);
	wait_queue_not_empty_.set();

	sprintf_s(msg, 4096, "[%s]�ѽ��˻��󶨡����յ��û�ȷ��������Ϣ�������", local_gbcode_.c_str());
	std::cout<<msg<<std::endl;
	app_->logger().information(msg);

	return 0;
}

int GxxGmDSJSimulater::SendBindUserConfirmInfo()
{
	// ����Ҫ�󹹽��ַ���
	const char *msg_format = 
		"<SubCmdType>BindUserConfirm</SubCmdType> \
		 <Receipt>1</Receipt> \
		 <PlatformID>%s</PlatformID> \
		 <DeviceIMEI>%s</DeviceIMEI> \
		 <UserID>%s</UserID> \
		 <Password>%s</Password>";

	// ������һ��MD5
	Poco::MD5Engine md5;
	md5.update(police_password_);
	std::string md5hex = md5.digestToHex(md5.digest());

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, 
		platform_id_.c_str(), imei_.c_str(), police_id_.c_str(), md5hex.c_str());

	ExtraDataResponseInfo info;
	info.extra_msg_ = msg;
	extra_response_queue_.push(info);
	wait_queue_not_empty_.set();

	sprintf_s(msg, 4096, "[%s]�ѽ��˻��󶨡���ȷ�ϰ�������Ϣ�������", local_gbcode_.c_str());
	std::cout<<msg<<std::endl;
	app_->logger().information(msg);

	return 0;
}

int GxxGmDSJSimulater::SendBaseInfo()
{
	// ����Ҫ�󹹽��ַ���
	const char *msg_format = "<SubCmdType>DeviceInfo</SubCmdType> \
							  <DeviceStates> \
									<Carrieroperator>%s</Carrieroperator> \
									<Nettype>%s</Nettype> \
									<Signal>%s</Signal> \
									<Battery>%s</Battery> \
									<Storage>%s</Storage> \
									<CPU>%s</CPU> \
									<Version>%s</Version> \
									<LocalRecord>%s</LocalRecord> \
							 </DeviceStates>";

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format,
		base_info_.carrieroperator_.c_str(), base_info_.net_type_.c_str(), base_info_.signal_.c_str(),
		base_info_.battery_.c_str(), base_info_.storage_.c_str(), base_info_.cpu_.c_str(),
		base_info_.version_.c_str(), base_info_.local_record_.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GS28181_ERR_SUCCESS;
	err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]�����豸������Ϣʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]�����豸������Ϣ�ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendLocationInfo()
{
	// ����Ҫ�󹹽��ַ���
	const char *msg_format = "<SubCmdType>LocationInfo</SubCmdType> \
							  <Location> \
									<DivisionNS>%s</DivisionNS> \
									<DivisionEW>%s</DivisionEW> \
									<Radius>%s</Radius > \
									<Longitude>%s</Longitude> \
									<Latitude>%s</Latitude> \
									<Direction>%s</Direction> \
									<Speed>%s</Speed> \
									<Height>%s</Height> \
									<LocationTime>%s</LocationTime> \
									<Satellites>%s</Satellites> \
									<LocationAvailable>%s</LocationAvailable> \
							 </Location>";

	// ����Ƚ��ر�ÿ�η�֮ǰ��ʱ������������¼���һ��
	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_time[128] = {0};
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// �������һ������λ��
	UpdateLocationPos();

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format,
		location_info_.division_ns_.c_str(), location_info_.division_ew_.c_str(), location_info_.radius_.c_str(),
		location_info_.longitude_.c_str(), location_info_.latitude_.c_str(), location_info_.direction_.c_str(),
		location_info_.speed_.c_str(), location_info_.height_.c_str(), current_time, location_info_.satellites_.c_str(),
		location_info_.location_available_.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]������չ�豸��λ��Ϣʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]������չ�豸��λ��Ϣ�ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendLocationInfoEx()
{
	// ����Ƚ��ر�ÿ�η�֮ǰ��ʱ������������¼���һ��
	char msg[4096] = {0};

	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_time[128] = {0};
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// �������һ������λ��
	UpdateLocationPos();

	StruMobilePosNotifyInfo position;
	strcpy_s(position.czGBCode, STR_GBCODE_LEN, local_gbcode_.c_str());
	strcpy_s(position.czDateTime, STR_DATETIME_LEN, current_time);
	position.dLongitude = atof(location_info_.longitude_.c_str());
	position.dLatitude = atof(location_info_.latitude_.c_str());
	position.dSpeed = atof(location_info_.speed_.c_str());
	position.dDirection = atof(location_info_.direction_.c_str());
	position.iAltitude = atoi(location_info_.height_.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	GS28181_ERR err = GB28181Agent_MobilePosNotify(agent_, mobile_position_sub_id_, &position);
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]���ͱ�׼�豸��λ��Ϣʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]���ͱ�׼�豸��λ��Ϣ�ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

// ���㻡��
double rad(double d)
{
	const double PI = 3.1415926535898;
	return d * PI / 180.0;
}

// ������gps����㣨��γ�ȣ���������ֱ�߾��룬��λ����
double CalcDistance(double fLati1, double fLong1, double fLati2, double fLong2)
{
	const float EARTH_RADIUS = 6378.137;


	double radLat1 = rad(fLati1);
	double radLat2 = rad(fLati2);
	double a = radLat1 - radLat2;
	double b = rad(fLong1) - rad(fLong2);
	double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
	s = s * EARTH_RADIUS;
	//s = (int)(s * 10000000) / 10000;
	s = s * 10000000 / 10000;
	return s;
}

int GxxGmDSJSimulater::UpdateLocationPos()
{
	// �����������λ�ƣ�����������������
	// 1. λ�ƾ���Ҫ����15��
	// 2. λ��ʱ�ٵ���137km/h����38m/s

	// ���������������ת����double��ת�Ƶ�С�����5λ
	double new_latitude = 0.0;
	double new_longtitude = 0.0;

	// �������ȡ�㣬�����������������ȡ��100��Ϊ����
	bool is_ok = false;
	for (int index = 0; index < 100; ++index)
	{
		Poco::Random random_object;
		double random_latitude = random_object.nextDouble() / 10000.0;
		double random_longtitude = random_object.nextDouble() / 10000.0;

		double new_latitude = last_latitude_ + random_latitude;
		double new_longtitude = last_longtitude_ + random_longtitude;

		// �����������
		double new_distance = CalcDistance(last_latitude_, last_longtitude_, new_latitude, new_longtitude);
		if (new_distance > 15)
		{
			// ����λ���ٶȣ��Ȼ�ȡʱ������ĿǰӦ���Ǻ���
			double speed = new_distance / (dev_location_time_ / 1000);
			if (speed > 38)
			{
				// ���������ˣ�����������Է���
				is_ok = true;
				break;
			}
		}
	}

	if (is_ok)
	{
		char tmp[4096] = {0};
		sprintf_s(tmp, 4096, "%f", new_latitude);
		location_info_.latitude_ = tmp;

		sprintf_s(tmp, 4096, "%f", new_longtitude);
		location_info_.longitude_ = tmp;
	}
	
	return 0;
}

int GxxGmDSJSimulater::SendExceptionInfo()
{
	const char *msg_format = "<SubCmdType>DeviceException</SubCmdType> \
							  <Exceptions> \
									<Storage>NotFound</Storage> \
									<Battery>Removed</Battery> \
									<CCD>Error</CCD> \
									<MIC>Error</MIC> \
							  </Exceptions>";

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format,
		exception_info_.storage_.c_str(), exception_info_.battery_.c_str(), exception_info_.ccd_.c_str(), exception_info_.mic_.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]�����豸�쳣��Ϣʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]���ͱ�׼�豸��λ��Ϣ�ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendAlarmInfo()
{
	const char *msg_format = "<SubCmdType>DeviceSOS</SubCmdType> \
							  <DeviceSOSInfo> \
									<Datetime>%s</Datetime> \
									<Location>  \
										<DivisionNS>%s</DivisionNS> \
										<DivisionEW>%s</DivisionEW> \
										<Radius>%s</Radius > \
										<Longitude>%s</Longitude> \
										<Latitude>%s</Latitude> \
										<Direction>%s</Direction> \
										<Speed>%s</Speed> \
										<Height>%s</Height> \
										<LocationTime>%s</LocationTime> \
										<Satellites>%s</Satellites> \
										<LocationAvailable>%s</LocationAvailable> \
									</Location> \
							</DeviceSOSInfo>";

	// ����Ƚ��ر�ÿ�η�֮ǰ��ʱ������������¼���һ��
	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_time[128] = {0};
	char current_datetime[128] = {0};
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d-%02d-%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	sprintf_s(current_datetime, 128, "%d-%02d-%02d %02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, current_datetime,
		location_info_.division_ns_.c_str(), location_info_.division_ew_.c_str(), location_info_.radius_.c_str(),
		location_info_.longitude_.c_str(), location_info_.latitude_.c_str(), location_info_.direction_.c_str(),
		location_info_.speed_.c_str(), location_info_.height_.c_str(), current_time, location_info_.satellites_.c_str(),
		location_info_.location_available_.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]�����豸�澯��Ϣʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]�����豸�澯��Ϣ�ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendFaceInfo(const char *face_img, int face_img_len)
{
	const char *msg_format = "<SubCmdType>FaceRecognition</SubCmdType> \
							  <FaceInfo> \
									<FaceImageData>%s</FaceImageData> \
									<DateTime>%s</DateTime> \
							  </FaceInfo>";

	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_datetime[128] = {0};
	sprintf_s(current_datetime, 128, "%d-%02d-%02d %02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	// ��������base64����
	std::ostringstream str;
	Poco::Base64Encoder encoder(str);
	encoder<<std::string(face_img, face_img_len);
	encoder.close();

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, str.str(), current_datetime);

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]��������ʶ������ʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]��������ʶ������ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendCarIdInfo()
{
	return -1;
	const char *msg_format = "<SubCmdType>CarIDRecognition</SubCmdType> \
							<CarIDInfo> \
							<CarIDImageData>1A2B3C4D5F1A2B3C4D5F��</CarIDImageData> \
							<DateTime>2018-08-15 18:15:07</DateTime> \
							</CarIDInfo>";

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format,
		base_info_.carrieroperator_.c_str(), base_info_.net_type_.c_str(), base_info_.signal_.c_str(),
		base_info_.battery_.c_str(), base_info_.storage_.c_str(), base_info_.cpu_.c_str(),
		base_info_.version_.c_str(), base_info_.local_record_.c_str());

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// �����Ƿ�Ҫ����һ�±�������
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]���ͳ���ʶ������ʧ�ܣ������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]���ͳ���ʶ������ɹ��������룺%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

void GxxGmDSJSimulater::_AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	// �������е�SIPͨ����־�����Կ���д����־�ļ�
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//char current_time[128] = {0};
	//sprintf_s(current_time, 128, "%d-%02d-%02d %02d-%02d-%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	//int len = iLen + 4096;
	//char *dbgmsg = new char[len];
	//sprintf_s(dbgmsg, len, "[%s]\n%s\n", current_time, szTemp);

	////// ��Ļ���
	////printf("%s", dbgmsg);

	//// dbgview���
	//OutputDebugStringA(dbgmsg);

	//// ��־�ļ����
	//DWORD written = 0;
	//WriteFile(simulater->log_file_handle_, dbgmsg, strlen(dbgmsg), &written, NULL);

	//delete [] dbgmsg;
	//dbgmsg = NULL;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData)
{
	// �����ﱻ��ѯĿ¼֮�����������̣߳���ʼ����28181��������
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	int errCode = 0;
	char dbg_msg[4096] = {0};
	char msg[4096] = {0};

	if (stuQuery == NULL)
		return SIP_RESPONSE_CODE_FAIL;

	if (stuQuery->eType == EnumQueryType::eQUE_DEV_INFO)
	{
		// �豸��Ϣ��ѯ
		StruDeviceInfo stuInfo;
		ZeroMemory(&stuInfo, sizeof(StruDeviceInfo));
		strcpy_s(stuInfo.czGBCode, STR_GBCODE_LEN, stuQuery->czGBCode);

		stuInfo.bResultOK = true;
		strcpy_s(stuInfo.czDeviceName, STR_NAME_LEN, "�����˹���ģ������ͷ");
		strcpy_s(stuInfo.czManufacturer, STR_MANUFACTURER_LEN, "GOSUNCN");
		strcpy_s(stuInfo.czModel, STR_MODEL_LEN, "GXX-GM-SIMULATE-001");
		strcpy_s(stuInfo.czFirmware, STR_FIREWARE_LEN, "V1.0, build 0001");
		stuInfo.iChanal = 1;

		GS28181_ERR err = GB28181Agent_RespondDevInfo(hSession, &stuInfo);
		if (err != GS28181_ERR_SUCCESS)
		{
			sprintf_s(msg, 4096, "[%s]��Ӧ�豸��Ϣ��ѯʧ�ܣ������룺%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_SUCCESS;
		}

		sprintf_s(msg, 4096, "[%s]��Ӧ�豸��Ϣ��ѯ�ɹ��������룺%d", simulater->local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_STATUS)
	{
		// �豸״̬��ѯ
		StruDeviceStatus stuStatus;
		ZeroMemory(&stuStatus, sizeof(StruDeviceStatus));
		strcpy_s(stuStatus.czGBCode, STR_GBCODE_LEN, stuQuery->czGBCode);

		SYSTEMTIME st;
		GetLocalTime(&st);
		char device_datetime[STR_DATETIME_LEN] = {0};
		sprintf_s(device_datetime, STR_DATETIME_LEN, "%d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		strcpy_s(stuStatus.czDevDateTime, STR_DATETIME_LEN, device_datetime);
		strcpy_s(stuStatus.czErrReason, STR_ERRREASON_LEN, "NO ERROR");
		stuStatus.bResult = true;
		stuStatus.bOnLine = true;
		stuStatus.bRecord = false;
		stuStatus.bEncode = false;
		stuStatus.bStatusOK = true;
		stuStatus.iAlarmNum = 1;

		StruAlarmStatus * stuAlarm = new StruAlarmStatus;
		ZeroMemory(stuAlarm, sizeof(StruAlarmStatus));
		strcpy_s(stuAlarm->czAlarmID, STR_ALARMID_LEN, simulater->local_gbcode_.c_str());	// �����豸����
		stuAlarm->eStatus = eONDUTY;

		stuStatus.ptrAlarm = stuAlarm;

		GS28181_ERR err = GB28181Agent_RespondDevStatus(hSession, &stuStatus);

		delete stuAlarm;
		stuAlarm = NULL;

		if (err != GS28181_ERR_SUCCESS)
		{
			sprintf_s(msg, 4096, "[%s]��Ӧ�豸״̬��ѯʧ�ܣ������룺%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_SUCCESS;

			sprintf_s(msg, 4096, "[%s]��Ӧ�豸״̬��ѯ�ɹ��������룺%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_CATALOG)
	{
		// �豸Ŀ¼��ѯ
		StruDeviceCatalog stuCata;
		strcpy_s(stuCata.czGBCode, STR_GBCODE_LEN, stuQuery->czGBCode);

		stuCata.iSumNum = 1;
		
		StruCatalogInfo *catlog = new StruCatalogInfo[stuCata.iSumNum];
		memset(catlog, 0, sizeof(StruCatalogInfo) * stuCata.iSumNum);

		strcpy_s(catlog[0].czGBCode, STR_GBCODE_LEN, simulater->local_gbcode_.c_str());
		strcpy_s(catlog[0].czDeviceName, STR_NAME_LEN, "�����˹���G4ִ����¼��");
		strcpy_s(catlog[0].czModel, STR_MODEL_LEN, "DSJ-G4");
		strcpy_s(catlog[0].czManufacturer, STR_MANUFACTURER_LEN, "�����˹����Ƽ����޹�˾");
		strcpy_s(catlog[0].czOwner, STR_OWNER_LEN, "440000");
		strcpy_s(catlog[0].czCivilCode, STR_CIVILCODE_LEN, "440100");
		strcpy_s(catlog[0].czBlock, STR_BLOCK_LEN, "���Ͻֵ��߸ھ���");
		strcpy_s(catlog[0].czAddress, STR_ADDRESS_LEN, "GuangZhou HuangpPu KaiChuang Road No.2819");
		catlog[0].iParental = 0;
		strcpy_s(catlog[0].czParentID, STR_PARENTID_LEN, simulater->local_gbcode_.c_str());
		catlog[0].iSafetyWay = 0;
		catlog[0].iRegisterWay = 1;
		strcpy_s(catlog[0].czCertNum, STR_CERTNUM_LEN, "CertNum 0");
		catlog[0].iCertifiable = 0;
		catlog[0].iErrCode = 400;
		strcpy_s(catlog[0].czEndTime, STR_DATETIME_LEN, "2010-11-11T19:46:17");
		catlog[0].iSecrecy = 0;
		strcpy_s(catlog[0].czIPAddress, STR_IPADDRESS_LEN, simulater->local_ip_.c_str());
		catlog[0].iPort = atoi(simulater->local_port_.c_str());
		strcpy_s(catlog[0].czPassword, STR_PASSWORD_LEN, "Password 0");
		catlog[0].iStatus = 1;
		catlog[0].dLongitude = 0;
		catlog[0].dLatitude = 0;
		catlog[0].iPtzType = 2;
		catlog[0].iRoomType = 3;

		stuCata.ptrCatalog = catlog;		

		GS28181_ERR err = GB28181Agent_RespondDevCatalog(hSession, &stuCata, 0, false);

		delete [] catlog;
		catlog = NULL;

		if(GS28181_ERR_SUCCESS != err)
		{
			// ��ѯĿ¼��Ӧʧ��
			sprintf_s(msg, 4096, "[%s]��Ӧ�豸Ŀ¼��ѯʧ�ܣ������룺%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_SUCCESS;
		}

		sprintf_s(msg, 4096, "[%s]��Ӧ�豸Ŀ¼��ѯ�ɹ��������룺%d", simulater->local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_RECORDINDEX)
	{
		// �豸¼���ѯ
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_CONFIGDOWNLOAD)
	{
		// �豸���ò�ѯ
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_PRESET)
	{
		// �豸Ԥ��λ��ѯ
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_DevControlCB(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_PlayControlCB(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_StreamRequestCB(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest, StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;
	char msg[4096] = {0};

	// �ȷ�����������Ϣ
	if (eRequest == eSTREAM_REALPLAY)
	{
#ifdef USE_REALSTREAM
		StruMediaInfo out_media_info;
		ZeroMemory(&out_media_info, sizeof(StruMediaInfo));

		// ����Ŀ���豸ID
		strncpy(out_media_info.czDeviceID, pInMedia->czDeviceID, STR_GBCODE_LEN);
		// ��������RTP�������IP��ַ
		strncpy(out_media_info.czIP, pInMedia->czIP, STR_IPADDRESS_LEN);

		// �����ֵʱ��Э������������������Play/Playback/download/Talk��
		strncpy(out_media_info.czMediaName, pInMedia->czMediaName, STR_NAME_LEN);

		// �Ƿ�֧��RTCP
		out_media_info.bEnableRTCP = pInMedia->bEnableRTCP;

		// ���䷽ʽ������Ҫת���ɱ��صģ�����������Ҫ��Rtp���ͷ�ת
		if (pInMedia->eRtpType == eRTP_OVER_UDP)
		{
			// RTP over UDP����
			out_media_info.eRtpType = eRTP_OVER_UDP;
		}
		else if (pInMedia->eRtpType == eRTP_OVER_TCP_ACTIVE)
		{
			// Զ����RTP over TCP��������
			// ���������Ӧ���Ǳ�������
			out_media_info.eRtpType = eRTP_OVER_TCP_PASSIVE;
		}
		else if (pInMedia->eRtpType = eRTP_OVER_TCP_PASSIVE)
		{
			// RTP over TCP��������
			out_media_info.eRtpType = eRTP_OVER_TCP_ACTIVE;
		}

		//��������  1:������  2:������
		out_media_info.iStreamType = pInMedia->iStreamType;

		// ��ʼUNIXʱ���
		out_media_info.iStart = pInMedia->iStart;
		// ����UNIXʱ���
		out_media_info.iEnd = pInMedia->iEnd;

		//�����������ʱ������ɲ���
		//Ϊ��֤SSRC�������ԣ�����ͳһ�о���Ҫôÿ�β��Ҫôÿ�ζ���
		out_media_info.iSSRC = pInMedia->iSSRC;

		out_media_info.stuDescri.iDescriNum = pInMedia->stuDescri.iDescriNum;
		for (int index = 0; index < out_media_info.stuDescri.iDescriNum; ++index)
		{
			out_media_info.stuDescri.mapDescri[index].eMediaType = pInMedia->stuDescri.mapDescri[index].eMediaType;
			out_media_info.stuDescri.mapDescri[index].iRtpmapNum = pInMedia->stuDescri.mapDescri[index].iRtpmapNum;
			for (int index2 = 0; index2 < out_media_info.stuDescri.mapDescri[index].iRtpmapNum; ++index2)
			{
				strncpy(out_media_info.stuDescri.mapDescri[index].mapRtp[index2].czMimeType, pInMedia->stuDescri.mapDescri[index].mapRtp[index2].czMimeType, STR_RTPTYPENAME_LEN);
				out_media_info.stuDescri.mapDescri[index].mapRtp[index2].iMediaFormat = pInMedia->stuDescri.mapDescri[index].mapRtp[index2].iMediaFormat;
				out_media_info.stuDescri.mapDescri[index].mapRtp[index2].iSampleRate = pInMedia->stuDescri.mapDescri[index].mapRtp[index2].iSampleRate;
			}
		}

		// ע��RTPʵʱ��
		unsigned short local_port = 0;
		int errCode = simulater->stream_mgr_.AddRealStream(hStream, pInMedia->iSSRC, local_port);
		out_media_info.iPort = local_port;

		// ���ϼ�ƽ̨�������
		GS28181_ERR err = GB28181Agent_RespondPlayRequest(hStream, eSTREAM_REALPLAY, &out_media_info, NULL);
		if (err != GS28181_ERR_SUCCESS)
		{
			// ���ص�����Ϣʧ�ܣ�
			sprintf_s(msg, 4096, "[%s]���ص�����Ϣʧ�ܣ������룺%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_FAIL;
		}

		// ��������ȷ���ˣ�����ȥ�Ķ˿ں������⣬����Զ�˵Ķ˿ں�
		simulater->stream_mgr_.StartRealStream(hStream, pInMedia->iSSRC, pInMedia->czIP, pInMedia->iPort);

#endif
	}
	else if (eRequest == eSTREAM_BYE)
	{
		simulater->stream_mgr_.StopRealStream();

		sprintf_s(msg, 4096, "[%s]�յ�ͣ������", simulater->local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;
	char msg[4096] = {0};

	switch (eType)
	{
	case EnumNotifyType::eNOTIFY_BROADCAST:
		// ���յ������㲥
		{
			sprintf_s(msg, 4096, "[%s]�յ������㲥����.", simulater->local_gbcode_.c_str());
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_CATASUBS:
		// ���յ�Ŀ¼����
		{
			StruCatalogSubInfo *sub_info = (StruCatalogSubInfo *)pMsg;

			sprintf_s(msg, 4096, "[%s]�յ�Ŀ¼�������󡣶���ID��%d", simulater->local_gbcode_.c_str(), sub_info->iID);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_ALARMSUBS:
		// ���յ��澯����
		{
			StruAlarmSubInfo *sub_info = (StruAlarmSubInfo *)pMsg;

			sprintf_s(msg, 4096, "[%s]�յ��澯�������󡣶���ID��%d", simulater->local_gbcode_.c_str(), sub_info->iSubID);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_MOBILEPOSSUB:
		{
			// ���յ��ƶ��豸��λ���ģ������̣߳����Ͷ�λ��Ϣ
			// ��ǿ�����λ�ϱ������߳�
			simulater->is_standard_gb28181_mobile_position_ = true;

			// �õ�������Ϣ����ȡ����ID
			StruMobilePosSubInfo *mobile_position_sub_info = (StruMobilePosSubInfo *)pMsg;
			simulater->mobile_position_sub_id_ = mobile_position_sub_info->iSubID;

			sprintf_s(msg, 4096, "[%s]�յ��ƶ��豸��λ�������󡣶���ID��%d", simulater->local_gbcode_.c_str(), simulater->mobile_position_sub_id_);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_SUBSEXPIRED:
		// ��ֹ����
		sprintf_s(msg, 4096, "[%s]�յ����Ĺ�������", simulater->local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		break;
	default:
		break;
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_ExtendRqeustCallBack(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;
	int errCode = 0;
	std::string errStr;
	char msg[4096] = {0};

	// ����������͸�����������ݣ��Ѿ�������Base64��
	// ��XML��һ����
	char *message = (char *)pMsg;

	// ����XMLͷβ�����ڽ���
	std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Info>\n";
	xml += message;
	xml += "\n</Info>";

	// ����Ҫ����һ��תUTF8
	Poco::Latin1Encoding latin1;
	Poco::UTF8Encoding utf8;
	Poco::TextConverter converter(latin1, utf8);
	std::string strUtf8;
	converter.convert(xml, strUtf8);
	xml = strUtf8;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError xml_err = doc.Parse(xml.c_str());
	if (xml_err != tinyxml2::XMLError::XML_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]�յ��޷�ʶ�����չ��Ϣ��%s", simulater->local_gbcode_.c_str(), xml.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		return SIP_RESPONSE_CODE_BAD_REQUEST;
	}

	tinyxml2::XMLElement *root = doc.RootElement();
	tinyxml2::XMLElement *person_SubCmdType = root->FirstChildElement();
	const char *sub_cmd_type = person_SubCmdType->GetText();

	// ����Э�鴦��
	if (_stricmp(sub_cmd_type, "BindUser") == 0)
	{
		// �ŷ�1���Ȼظ����յ������������·�
		// ����2���ں���ֱ�Ӵ��λظ�

		// ���ж������Ƿ���ȷ
		tinyxml2::XMLElement *element_sub_cmd_result = root->FirstChildElement("SubCmdResult");
		const char *sub_cmd_result = element_sub_cmd_result->GetText();
		if (_stricmp(sub_cmd_result, "FAILED") == 0)
		{
			// ��ʧ���ˣ����������ӿ�ͨ��������Ҳ��ʾһ��
			tinyxml2::XMLElement *element_sub_cmd_result_detail = root->FirstChildElement("SubCmdResultDetail");
			const char *sub_cmd_result_detail = element_sub_cmd_result_detail->GetText();

			simulater->notifer_->RecvBindUser(sub_cmd_result_detail);
			//simulater->speaker_->Speak("�˺Ż��������");
			sprintf_s(msg, 4096, "[%s]�յ��˻��󶨷�����%s", simulater->local_gbcode_.c_str(), sub_cmd_result_detail);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
			
			return SIP_RESPONSE_CODE_SUCCESS;
		}

		// ������û����󣬵õ�ƽ̨���صĲ������������ã����������óɹ�
		tinyxml2::XMLElement *element_update_setting = root->FirstChildElement("UpdateSetting");

		{
			tinyxml2::XMLElement *element_platform_setting = element_update_setting->FirstChildElement("PlatformSetting");

			tinyxml2::XMLElement *element_device_name = element_platform_setting->FirstChildElement("DeviceName");
			tinyxml2::XMLElement *element_frame_rate = element_platform_setting->FirstChildElement("FrameRate");
			tinyxml2::XMLElement *element_bit_rate = element_platform_setting->FirstChildElement("BitRate");
			tinyxml2::XMLElement *element_trans_resolution = element_platform_setting->FirstChildElement("TransResolution");

			simulater->dev_name_ = element_device_name->GetText();
			simulater->frame_rate_ = element_frame_rate->GetText();
			simulater->bit_rate_ = element_bit_rate->GetText();
			simulater->trans_resolution_ = element_trans_resolution->GetText();
		}
		
		{
			tinyxml2::XMLElement *element_osd_setting = element_update_setting->FirstChildElement("OSDSetting");

			tinyxml2::XMLElement *element_machine_id = element_osd_setting->FirstChildElement("MachineID");
			tinyxml2::XMLElement *element_user_name = element_osd_setting->FirstChildElement("Username");
			tinyxml2::XMLElement *element_user_id = element_osd_setting->FirstChildElement("UserID");
			tinyxml2::XMLElement *element_dep_name = element_osd_setting->FirstChildElement("DepName");
			tinyxml2::XMLElement *element_dep_id = element_osd_setting->FirstChildElement("DepID");
			tinyxml2::XMLElement *element_location = element_osd_setting->FirstChildElement("Location");
			tinyxml2::XMLElement *element_time = element_osd_setting->FirstChildElement("Time");

			simulater->machine_id_ = element_machine_id->GetText();
			simulater->bind_user_name_ = element_user_name->GetText();
			simulater->bind_user_id_ = element_user_id->GetText();
			simulater->dep_name_ = element_dep_name->GetText();
			simulater->dep_id_ = element_dep_id->GetText();
			simulater->show_location_ = element_location->GetText();
			simulater->show_datetime_ = element_time->GetText();
		}

		{
			tinyxml2::XMLElement *element_rate = element_update_setting->FirstChildElement("Rate");

			tinyxml2::XMLElement *element_location_rate = element_rate->FirstChildElement("LocationRate");
			tinyxml2::XMLElement *element_device_states_rate = element_rate->FirstChildElement("DeviceStatesRate");

			simulater->dev_location_time_ = atoi(element_location_rate->GetText());
			simulater->dev_baseinfo_time_ = atoi(element_device_states_rate->GetText());
		}

		// ����Ӧ����������˻��󶨵ģ�Ӧ����¼һ����־
		sprintf_s(msg, 4096, "[%s]�յ��˻��󶨷������󶨳ɹ���", simulater->local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		// �յ����û���Ϣ�Ľ��
		// �����ǲ���Ӧ���ӵ�һ���������洦��

		// ���ͳɹ���Ϣ
		const char *result = "<SubCmdType>ConfigUpdateReceipt</SubCmdType>\
			<ConfigUpdateReport>\
			<UpdateResult>SUCCESS</UpdateResult>\
			<UpdateResultDetail>OK</UpdateResultDetail>\
			<Callbacks>\
			<CallbackURL></CallbackURL>\
			</Callbacks>\
			</ConfigUpdateReport>";

		ExtraDataResponseInfo response;
		response.target_device_id_ = czTargetDevID;
		response.extra_msg_ = result;
		
		// �������
		simulater->extra_response_queue_.push(response);
		simulater->wait_queue_not_empty_.set();
	}
	else if (_stricmp(sub_cmd_type, "BindUserConfirm") == 0)
	{
		sprintf_s(msg, 4096, "[%s]�յ��˻���ȷ����Ϣ�����û�֮ǰ�Ѱ��������豸...", simulater->local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		// ���յ��û�ȷ�ϰ�������Ͳ������ˣ�ֱ�ӷ���һ������
		simulater->SendBindUserConfirmRecevicedInfo();
		simulater->SendBindUserConfirmInfo();
	}
	else if (_stricmp(sub_cmd_type, "ConfigUpdateReceipt") == 0)
	{
		// �˻��󶨻ص������������Բ��ù�
	}
	else if (_stricmp(sub_cmd_type, "DeviceInfo") == 0)
	{
		// �豸������Ϣ�ϱ������������Բ��ù�
	}
	else if (_stricmp(sub_cmd_type, "LocationInfo") == 0)
	{
		// �豸��λ��Ϣ�ϱ������������Բ��ù�
	}
	else if (_stricmp(sub_cmd_type, "DeviceException") == 0)
	{
		// �豸�쳣��Ϣ�ϱ������������Բ��ù�
	}
	else if (_stricmp(sub_cmd_type, "RemoteBindUser") == 0)
	{
		// ƽ̨�·����û��������ݲ�ʵ��
	}
	else if (_stricmp(sub_cmd_type, "RemoteRecord") == 0)
	{
		// ƽ̨�·�Զ��¼�������ݲ�ʵ��
	}
	else if (_stricmp(sub_cmd_type, "RemoteTakePhoto") == 0)
	{
		// ƽ̨�·�Զ�����գ������ݲ�ʵ��
	}
	else if (_stricmp(sub_cmd_type, "RemoteDeviceLock") == 0)
	{
		// ƽ̨�·�Զ�������������ݲ�ʵ��
	}
	else if (_stricmp(sub_cmd_type, "RemoteUpdateSetting") == 0)
	{
		// ƽ̨�·��������ã������ݲ�ʵ��
	}
	else if (_stricmp(sub_cmd_type, "EmergencyInfo") == 0)
	{
		// ƽ̨�·�������Ϣ
		tinyxml2::XMLElement *element_emergency_id = root->FirstChildElement("EmergencyId");
		tinyxml2::XMLElement *element_dispatch_time = root->FirstChildElement("DispatchTime");
		tinyxml2::XMLElement *element_dispatch_end_time = root->FirstChildElement("DispatchEndTime");
		
		const char *emergency_id = element_emergency_id->GetText();
		const char *dispatch_time = element_dispatch_time->GetText();
		const char *dispatch_end_time = element_dispatch_end_time->GetText();

		simulater->notifer_->RecvEmergency(emergency_id, dispatch_time, dispatch_end_time);

		// �����������
		sprintf_s(msg, 4096, "[%s]�յ�������Ϣ\n����ID��%s\n����ʱ�䣺%s\n��������ʱ��%s", simulater->local_gbcode_.c_str(), emergency_id, dispatch_time, dispatch_end_time);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}
	else if (_stricmp(sub_cmd_type, "P2DUpdate") == 0)
	{
		// 2019-06-21 10:00��ʼʵ�� - wangy
		// ƽ̨�·���������
		tinyxml2::XMLElement *element_package_name = root->FirstChildElement("PackageName");
		tinyxml2::XMLElement *element_hash_code = root->FirstChildElement("HASHCODE");
		tinyxml2::XMLElement *element_hash_class = root->FirstChildElement("HASHCLASS");
		tinyxml2::XMLElement *element_update_level = root->FirstChildElement("UpdateLevel");
		tinyxml2::XMLElement *element_newest_version = root->FirstChildElement("NewestVersion");
		tinyxml2::XMLElement *element_newest_package_url = root->FirstChildElement("NewestPackageUrl");

		const char *package_name		= element_package_name->GetText();
		const char *hash_code			= element_hash_code->GetText();
		const char *hash_class			= element_hash_class->GetText();
		const char *update_level		= element_update_level->GetText();
		const char *newest_version		= element_newest_version->GetText();
		const char *newest_package_url	= element_newest_package_url->GetText();

		// ���ͳɹ���Ϣ
		const char *result = "<SubCmdType>P2DUpdate</SubCmdType>"
							"<SubCmdResult>SUCCEES</SubCmdResult>";

		ExtraDataResponseInfo response;
		response.target_device_id_ = czTargetDevID;
		response.extra_msg_ = result;

		// �������
		simulater->extra_response_queue_.push(response);
		simulater->wait_queue_not_empty_.set();

		// ��¼��־
		sprintf_s(msg, 4096, "[%s]�յ��豸��������\n������%s\n��ժҪ��%s\nժҪ�㷨��%s\n������Χ��%s\n�������汾��%s\n���������ص�ַ��%s", 
			simulater->local_gbcode_.c_str(), package_name, hash_code, hash_class, update_level, newest_version, newest_package_url);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}

void GxxGmDSJSimulater::RecvBindUser(const char *result)
{
	//printf("�յ��û��󶨽����%s\n", result);
}

void GxxGmDSJSimulater::RecvEmergency(const char *emergency_id, const char *start_time, const char *end_time)
{
	// û�����ù۲��ߵ�ʱ��ָ���Լ���ʲô������
	//printf("�յ��¾���������ID��%s������ʱ�䣺%s����������ʱ�䣺%s\n", emergency_id, start_time, end_time);
}

void GxxGmDSJSimulater::RecvRemoteBindUser()
{
	// û�����ù۲��ߵ�ʱ��ָ���Լ���ʲô������
}

void GxxGmDSJSimulater::GB28181HeartbeatThreadFun(void *param)
{
	// 
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)param;
	char msg[4096] = {0};

	int errCode = 0;
	std::string errStr;

	try
	{
		StruConnectParam connention_param;
		strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, simulater->server_ip_.c_str());
		strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, simulater->server_gbcode_.c_str());
		connention_param.iPort = atoi(simulater->server_port_.c_str());

		int heartbeat_count = simulater->gb28181_hb_time_;
		int baseinfo_count = simulater->dev_baseinfo_time_;
		int location_count = simulater->dev_location_time_;
		int userbind_count = simulater->dev_userbind_time_;

		while (!simulater->is_gb28181_heartbeat_thread_need_exit_)
		{
			
			if (heartbeat_count == simulater->gb28181_hb_time_)
			{
				// ���ͱ�������
				StruErrorList *error_list = NULL;

				GS28181_ERR err = GS28181_ERR_SUCCESS;
				err = GB28181Agent_HeartBeat(simulater->agent_, &connention_param, 1, NULL);
				if (err != GS28181_ERR_SUCCESS)
				{
					sprintf_s(msg, 4096, "[%s] ����GB28181��������ʧ�ܡ������룺%d", simulater->local_gbcode_.c_str(), err);
					std::cout<<msg<<std::endl;
					simulater->app_->logger().error(msg);
				}

				heartbeat_count = 0;
			}

			if (baseinfo_count == simulater->dev_baseinfo_time_)
			{
				// �����豸������Ϣ
				simulater->SendBaseInfo();
				baseinfo_count = 0;
			}

			if (location_count == simulater->dev_location_time_)
			{
				// ���Ͷ�λ��Ϣ
				if (simulater->is_standard_gb28181_mobile_position_)
				{
					// ���ͱ�׼GB28181-2016��λ��Ϣ
					simulater->SendLocationInfoEx();
				}
				else
				{
					// ������չ��λ��Ϣ
					simulater->SendLocationInfo();
				}
				location_count = 0;
			}

			if (userbind_count == simulater->dev_userbind_time_)
			{
				simulater->SendBindUserInfo(simulater->police_id_.c_str(), simulater->police_password_.c_str());
				userbind_count = 0;
			}

			// ��1�������
			Poco::Thread::sleep(1);
			//Sleep(1);
			++heartbeat_count;
			++baseinfo_count;
			++location_count;
			++userbind_count;

		}
	}
	catch (Poco::Exception e)
	{
		errCode = e.code();
		errStr = e.displayText();
	}
}

void GxxGmDSJSimulater::ExtraDataResponseThread(void *param)
{
	GxxGmDSJSimulater *simulator = (GxxGmDSJSimulater *)param;
	char msg[4096] = {0};

	// ���ýӿڣ�����͸����Ϣ
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, simulator->server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, simulator->server_gbcode_.c_str());
	connention_param.iPort = atoi(simulator->server_port_.c_str());

	while (true)
	{
		// ͨ���¼��������Ƿ�Ϊ��
		if (simulator->extra_response_queue_.empty())
		{
			//simulator->wait_queue_not_empty_.wait(10);
			Poco::Thread::sleep(5);
			continue;
		}

		ExtraDataResponseInfo extra_data = simulator->extra_response_queue_.front();
		simulator->extra_response_queue_.pop();

		// �����Ƿ�Ҫ����һ�±�������
		GS28181_ERR err = GB28181Agent_NotifyTransData(simulator->agent_, &connention_param, simulator->local_gbcode_.c_str(), extra_data.extra_msg_.c_str(), extra_data.extra_msg_.size());
		if (err != GS28181_ERR_SUCCESS)
		{
			sprintf_s(msg, 4096, "[%s]��������ʧ�ܣ������룺%d��������Ϣ��\n%s\n", simulator->local_gbcode_.c_str(), err, extra_data.extra_msg_.c_str());
			std::cout<<msg<<std::endl;
			simulator->app_->logger().error(msg);
		}
		else
		{
			sprintf_s(msg, 4096, "[%s]��������ɹ��������룺%d��������Ϣ��\n%s\n", simulator->local_gbcode_.c_str(), err, extra_data.extra_msg_.c_str());
			std::cout<<msg<<std::endl;
			simulator->app_->logger().information(msg);
		}
	}
	
}