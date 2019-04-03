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

	// 初始化协议栈
	//agent_ = GB28181Agent_Init(2, 16, Enum28181Version::eVERSION_2016, 3000);
	agent_ = GB28181Agent_Init(1, 1, Enum28181Version::eVERSION_2016, 3000);
	if (agent_ == NULL)
	{
		sprintf_s(msg, 4096, "[%s]初始化28181协议栈失败！", init_info.local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
		return -1;
	}

	// 安装回调
	GB28181Agent_SetLogCB(agent_, _AgentLogCallBack, this);
	GB28181Agent_SetDevInfoQueryCB(agent_, _DevInfoQueryCB, this);
	GB28181Agent_SetDevControlCB(agent_, _DevControlCB, this);
	GB28181Agent_SetPlayControlCB(agent_, _PlayControlCB, this);
	GB28181Agent_SetStreamRequestCB(agent_, _StreamRequestCB, this);
	GB28181Agent_SetNotifyInfoCB(agent_, _NotifyInfo_CallBackFunc, this);
	GB28181Agent_SetExtendRequestCB(agent_, _ExtendRqeustCallBack, this);

	// 这里判断网络情况
	EnumTransType eTransType = EnumTransType::eTYPE_UDP;
	if (init_info.sip_net_.compare("UDP") == 0)
		eTransType = EnumTransType::eTYPE_UDP;
	else if (init_info.sip_net_.compare("TCP") == 0)
		eTransType = EnumTransType::eTYPE_TCP;

	GS28181_ERR err = GB28181Agent_Start(agent_, init_info.local_ip_.c_str(), atoi(init_info.local_port_.c_str()), init_info.local_gbcode_.c_str(), eTransType);
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]启动28181协议栈失败！错误码：%d", init_info.local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);

		GB28181Agent_Uninit(agent_);
		return err;
	}

	reg_msg.iExpires = 86400;	// 1年
	strcpy_s(reg_msg.szUserName, STR_USERNAME_LEN, init_info.username_.c_str());
	strcpy_s(reg_msg.szPassword, STR_PASSWORD_LEN, init_info.password_.c_str());
	strcpy_s(reg_msg.stuCnnParam.szIP, STR_IPADDRESS_LEN, init_info.server_ip_.c_str());
	reg_msg.stuCnnParam.iPort = (unsigned int)atoi(init_info.server_port_.c_str());
	strcpy_s(reg_msg.stuCnnParam.szGBCode, STR_GBCODE_LEN, init_info.server_gbcode_.c_str());

	char date_time[4096] = {0};
	err = GB28181Agent_Register(agent_, &reg_msg, date_time);
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]注册到接入网关失败！错误码：%d", init_info.local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);

		GB28181Agent_Stop(agent_);
		GB28181Agent_Uninit(agent_);
		return err;
	}

	sprintf_s(msg, 4096, "[%s]注册到接入网关成功！错误码：%d", init_info.local_gbcode_.c_str(), err);
	std::cout<<msg<<std::endl;
	app_->logger().information(msg);

	// 保存参数
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

	// 设置默认名称
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

	// 初始化流管理模块
	stream_mgr_.SetFFmpegStub(ffmpeg_stub);
	errCode = stream_mgr_.Initialize(init_info.manual_port_, init_info.begin_port_, init_info.end_port_, init_info.local_ip_.c_str(), init_info.rtp_net_.c_str(), init_info.stream_file_.c_str());
	if (errCode != 0)
	{
		sprintf_s(msg, 4096, "[%s]初始化推流服务失败！", init_info.local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]初始化推流服务成功！", init_info.local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	// 查询目录响应成功
	// 开始向上推送设备状态信息和定位信息
	// 使用Poco的多线程框架做吧
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
	// 先发送命令，停止心跳发送，并且停止基础信息、定位信息的推送
	is_gb28181_heartbeat_thread_need_exit_ = true;
	gb28181_heartbeat_thread_.join();

	// 停止推流
	stream_mgr_.StopRealStream();

	// 反注册
	StruRegistMsg reg_msg;

	reg_msg.iExpires = 0;	// 1年
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
	// 按照要求构建字符串
	const char *msg_format = "<SubCmdType>BindUser</SubCmdType> \
							  <DeviceStates> \
									<PlatformID>%s</PlatformID> \
									<DeviceIMEI>%s</DeviceIMEI> \
									<UserID>%s</UserID> \
									<Password>%s</Password> \
							  </DeviceStates>";

	// 保存一下警号与密码
	police_id_ = user_id;
	police_password_ = password;

	// 这里算一波MD5
	Poco::MD5Engine md5;
	md5.update(password);
	std::string md5hex = md5.digestToHex(md5.digest());
	//std::string md5hex = password;

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, 
		platform_id_.c_str(), imei_.c_str(), user_id, md5hex.c_str());

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]发送人机绑定请求失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送人机绑定请求成功！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendBindUserConfirmRecevicedInfo()
{
	// 将参数加入队列，有线程专门处理消息发送
	GS28181_ERR err = GS28181_ERR_SUCCESS;

	char msg[4096] = {0};
	strcpy_s(msg, 4096, "<SubCmdType>BindUserConfirmReceived</SubCmdType>");

	ExtraDataResponseInfo info;
	info.extra_msg_ = msg;
	extra_response_queue_.push(info);
	wait_queue_not_empty_.set();

	sprintf_s(msg, 4096, "[%s]已将人机绑定——收到用户确认请求消息放入队列", local_gbcode_.c_str());
	std::cout<<msg<<std::endl;
	app_->logger().information(msg);

	return 0;
}

int GxxGmDSJSimulater::SendBindUserConfirmInfo()
{
	// 按照要求构建字符串
	const char *msg_format = 
		"<SubCmdType>BindUserConfirm</SubCmdType> \
		 <Receipt>1</Receipt> \
		 <PlatformID>%s</PlatformID> \
		 <DeviceIMEI>%s</DeviceIMEI> \
		 <UserID>%s</UserID> \
		 <Password>%s</Password>";

	// 这里算一波MD5
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

	sprintf_s(msg, 4096, "[%s]已将人机绑定——确认绑定请求消息放入队列", local_gbcode_.c_str());
	std::cout<<msg<<std::endl;
	app_->logger().information(msg);

	return 0;
}

int GxxGmDSJSimulater::SendBaseInfo()
{
	// 按照要求构建字符串
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

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GS28181_ERR_SUCCESS;
	err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]发送设备基础信息失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送设备基础信息成功！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendLocationInfo()
{
	// 按照要求构建字符串
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

	// 这里比较特别，每次发之前，时间我们最好重新计算一下
	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_time[128] = {0};
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format,
		location_info_.division_ns_.c_str(), location_info_.division_ew_.c_str(), location_info_.radius_.c_str(),
		location_info_.longitude_.c_str(), location_info_.latitude_.c_str(), location_info_.direction_.c_str(),
		location_info_.speed_.c_str(), location_info_.height_.c_str(), current_time, location_info_.satellites_.c_str(),
		location_info_.location_available_.c_str());

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]发送扩展设备定位信息失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送扩展设备定位信息成功！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

int GxxGmDSJSimulater::SendLocationInfoEx()
{
	// 这里比较特别，每次发之前，时间我们最好重新计算一下
	char msg[4096] = {0};

	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_time[128] = {0};
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	StruMobilePosNotifyInfo position;
	strcpy_s(position.czGBCode, STR_GBCODE_LEN, local_gbcode_.c_str());
	strcpy_s(position.czDateTime, STR_DATETIME_LEN, current_time);
	position.dLongitude = atof(location_info_.longitude_.c_str());
	position.dLatitude = atof(location_info_.latitude_.c_str());
	position.dSpeed = atof(location_info_.speed_.c_str());
	position.dDirection = atof(location_info_.direction_.c_str());
	position.iAltitude = atoi(location_info_.height_.c_str());

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	GS28181_ERR err = GB28181Agent_MobilePosNotify(agent_, mobile_position_sub_id_, &position);
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]发送标准设备定位信息失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送标准设备定位信息成功！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
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

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]发送设备异常信息失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送标准设备定位信息成功！错误码：%d", local_gbcode_.c_str(), err);
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

	// 这里比较特别，每次发之前，时间我们最好重新计算一下
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

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]发送设备告警信息失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送设备告警信息成功！错误码：%d", local_gbcode_.c_str(), err);
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

	// 人脸数据base64编码
	std::ostringstream str;
	Poco::Base64Encoder encoder(str);
	encoder<<std::string(face_img, face_img_len);
	encoder.close();

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, str.str(), current_datetime);

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		sprintf_s(msg, 4096, "[%s]发送人脸识别请求失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送人脸识别请求成功！错误码：%d", local_gbcode_.c_str(), err);
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
							<CarIDImageData>1A2B3C4D5F1A2B3C4D5F…</CarIDImageData> \
							<DateTime>2018-08-15 18:15:07</DateTime> \
							</CarIDInfo>";

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format,
		base_info_.carrieroperator_.c_str(), base_info_.net_type_.c_str(), base_info_.signal_.c_str(),
		base_info_.battery_.c_str(), base_info_.storage_.c_str(), base_info_.cpu_.c_str(),
		base_info_.version_.c_str(), base_info_.local_record_.c_str());

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, server_gbcode_.c_str());
	connention_param.iPort = atoi(server_port_.c_str());

	// 这里是否要考虑一下编码问题
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		sprintf_s(msg, 4096, "[%s]发送车牌识别请求失败！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().error(msg);
	}
	else
	{
		sprintf_s(msg, 4096, "[%s]发送车牌识别请求成功！错误码：%d", local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		app_->logger().information(msg);
	}

	return err;
}

void GxxGmDSJSimulater::_AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	// 这是所有的SIP通信日志，可以考虑写入日志文件
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//char current_time[128] = {0};
	//sprintf_s(current_time, 128, "%d-%02d-%02d %02d-%02d-%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	//int len = iLen + 4096;
	//char *dbgmsg = new char[len];
	//sprintf_s(dbgmsg, len, "[%s]\n%s\n", current_time, szTemp);

	////// 屏幕输出
	////printf("%s", dbgmsg);

	//// dbgview输出
	//OutputDebugStringA(dbgmsg);

	//// 日志文件输出
	//DWORD written = 0;
	//WriteFile(simulater->log_file_handle_, dbgmsg, strlen(dbgmsg), &written, NULL);

	//delete [] dbgmsg;
	//dbgmsg = NULL;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData)
{
	// 当这里被查询目录之后，立即启动线程，开始发送28181保活心跳
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	int errCode = 0;
	char dbg_msg[4096] = {0};
	char msg[4096] = {0};

	if (stuQuery == NULL)
		return SIP_RESPONSE_CODE_FAIL;

	if (stuQuery->eType == EnumQueryType::eQUE_DEV_INFO)
	{
		// 设备信息查询
		StruDeviceInfo stuInfo;
		ZeroMemory(&stuInfo, sizeof(StruDeviceInfo));
		strcpy_s(stuInfo.czGBCode, STR_GBCODE_LEN, stuQuery->czGBCode);

		stuInfo.bResultOK = true;
		strcpy_s(stuInfo.czDeviceName, STR_NAME_LEN, "高新兴国迈模拟摄像头");
		strcpy_s(stuInfo.czManufacturer, STR_MANUFACTURER_LEN, "GOSUNCN");
		strcpy_s(stuInfo.czModel, STR_MODEL_LEN, "GXX-GM-SIMULATE-001");
		strcpy_s(stuInfo.czFirmware, STR_FIREWARE_LEN, "V1.0, build 0001");
		stuInfo.iChanal = 1;

		GS28181_ERR err = GB28181Agent_RespondDevInfo(hSession, &stuInfo);
		if (err != GS28181_ERR_SUCCESS)
		{
			sprintf_s(msg, 4096, "[%s]响应设备信息查询失败！错误码：%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_SUCCESS;
		}

		sprintf_s(msg, 4096, "[%s]响应设备信息查询成功！错误码：%d", simulater->local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_STATUS)
	{
		// 设备状态查询
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
		strcpy_s(stuAlarm->czAlarmID, STR_ALARMID_LEN, simulater->local_gbcode_.c_str());	// 报警设备编码
		stuAlarm->eStatus = eONDUTY;

		stuStatus.ptrAlarm = stuAlarm;

		GS28181_ERR err = GB28181Agent_RespondDevStatus(hSession, &stuStatus);

		delete stuAlarm;
		stuAlarm = NULL;

		if (err != GS28181_ERR_SUCCESS)
		{
			sprintf_s(msg, 4096, "[%s]响应设备状态查询失败！错误码：%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_SUCCESS;

			sprintf_s(msg, 4096, "[%s]响应设备状态查询成功！错误码：%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_CATALOG)
	{
		// 设备目录查询
		StruDeviceCatalog stuCata;
		strcpy_s(stuCata.czGBCode, STR_GBCODE_LEN, stuQuery->czGBCode);

		stuCata.iSumNum = 1;
		
		StruCatalogInfo *catlog = new StruCatalogInfo[stuCata.iSumNum];
		memset(catlog, 0, sizeof(StruCatalogInfo) * stuCata.iSumNum);

		strcpy_s(catlog[0].czGBCode, STR_GBCODE_LEN, simulater->local_gbcode_.c_str());
		strcpy_s(catlog[0].czDeviceName, STR_NAME_LEN, "高新兴国迈G4执法记录仪");
		strcpy_s(catlog[0].czModel, STR_MODEL_LEN, "DSJ-G4");
		strcpy_s(catlog[0].czManufacturer, STR_MANUFACTURER_LEN, "高新兴国迈科技有限公司");
		strcpy_s(catlog[0].czOwner, STR_OWNER_LEN, "440000");
		strcpy_s(catlog[0].czCivilCode, STR_CIVILCODE_LEN, "440100");
		strcpy_s(catlog[0].czBlock, STR_BLOCK_LEN, "联合街道暹岗警区");
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
			// 查询目录响应失败
			sprintf_s(msg, 4096, "[%s]响应设备目录查询失败！错误码：%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_SUCCESS;
		}

		sprintf_s(msg, 4096, "[%s]响应设备目录查询成功！错误码：%d", simulater->local_gbcode_.c_str(), err);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_RECORDINDEX)
	{
		// 设备录像查询
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_CONFIGDOWNLOAD)
	{
		// 设备配置查询
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_PRESET)
	{
		// 设备预置位查询
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

	// 先分析输入流信息
	if (eRequest == eSTREAM_REALPLAY)
	{
#ifdef USE_REALSTREAM
		StruMediaInfo out_media_info;
		ZeroMemory(&out_media_info, sizeof(StruMediaInfo));

		// 点流目标设备ID
		strncpy(out_media_info.czDeviceID, pInMedia->czDeviceID, STR_GBCODE_LEN);
		// 本地用于RTP流传输的IP地址
		strncpy(out_media_info.czIP, pInMedia->czIP, STR_IPADDRESS_LEN);

		// 不填该值时，协议会根据请求类型填入Play/Playback/download/Talk等
		strncpy(out_media_info.czMediaName, pInMedia->czMediaName, STR_NAME_LEN);

		// 是否支持RTCP
		out_media_info.bEnableRTCP = pInMedia->bEnableRTCP;

		// 传输方式，由于要转换成本地的，所以这样需要将Rtp类型翻转
		if (pInMedia->eRtpType == eRTP_OVER_UDP)
		{
			// RTP over UDP传输
			out_media_info.eRtpType = eRTP_OVER_UDP;
		}
		else if (pInMedia->eRtpType == eRTP_OVER_TCP_ACTIVE)
		{
			// 远端是RTP over TCP主动传输
			// 本地这里就应该是被动传输
			out_media_info.eRtpType = eRTP_OVER_TCP_PASSIVE;
		}
		else if (pInMedia->eRtpType = eRTP_OVER_TCP_PASSIVE)
		{
			// RTP over TCP被动传输
			out_media_info.eRtpType = eRTP_OVER_TCP_ACTIVE;
		}

		//码流类型  1:主码流  2:副码流
		out_media_info.iStreamType = pInMedia->iStreamType;

		// 开始UNIX时间戳
		out_media_info.iStart = pInMedia->iStart;
		// 结束UNIX时间戳
		out_media_info.iEnd = pInMedia->iEnd;

		//发起点流请求时，可填可不填
		//为保证SSRC的连续性，必须统一行径，要么每次不填，要么每次都填
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

		// 注册RTP实时流
		unsigned short local_port = 0;
		int errCode = simulater->stream_mgr_.AddRealStream(hStream, pInMedia->iSSRC, local_port);
		out_media_info.iPort = local_port;

		// 向上级平台反馈结果
		GS28181_ERR err = GB28181Agent_RespondPlayRequest(hStream, eSTREAM_REALPLAY, &out_media_info, NULL);
		if (err != GS28181_ERR_SUCCESS)
		{
			// 返回点流信息失败！
			sprintf_s(msg, 4096, "[%s]返回点流信息失败！错误码：%d", simulater->local_gbcode_.c_str(), err);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().error(msg);

			return SIP_RESPONSE_CODE_FAIL;
		}

		// 启动流，确认了，推上去的端口号有问题，不是远端的端口号
		simulater->stream_mgr_.StartRealStream(hStream, pInMedia->iSSRC, pInMedia->czIP, pInMedia->iPort);

#endif
	}
	else if (eRequest == eSTREAM_BYE)
	{
		simulater->stream_mgr_.StopRealStream();

		sprintf_s(msg, 4096, "[%s]收到停流请求！", simulater->local_gbcode_.c_str());
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
		// 接收到语音广播
		{
			sprintf_s(msg, 4096, "[%s]收到语音广播请求.", simulater->local_gbcode_.c_str());
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_CATASUBS:
		// 接收到目录订阅
		{
			StruCatalogSubInfo *sub_info = (StruCatalogSubInfo *)pMsg;

			sprintf_s(msg, 4096, "[%s]收到目录订阅请求。订阅ID：%d", simulater->local_gbcode_.c_str(), sub_info->iID);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_ALARMSUBS:
		// 接收到告警订阅
		{
			StruAlarmSubInfo *sub_info = (StruAlarmSubInfo *)pMsg;

			sprintf_s(msg, 4096, "[%s]收到告警订阅请求。订阅ID：%d", simulater->local_gbcode_.c_str(), sub_info->iSubID);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_MOBILEPOSSUB:
		{
			// 接收到移动设备定位订阅，启动线程，推送定位信息
			// 标记开启定位上报工作线程
			simulater->is_standard_gb28181_mobile_position_ = true;

			// 得到订阅信息，提取订阅ID
			StruMobilePosSubInfo *mobile_position_sub_info = (StruMobilePosSubInfo *)pMsg;
			simulater->mobile_position_sub_id_ = mobile_position_sub_info->iSubID;

			sprintf_s(msg, 4096, "[%s]收到移动设备定位订阅请求。订阅ID：%d", simulater->local_gbcode_.c_str(), simulater->mobile_position_sub_id_);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
		}
		break;
	case EnumNotifyType::eNOTIFY_SUBSEXPIRED:
		// 终止订阅
		sprintf_s(msg, 4096, "[%s]收到订阅过期请求。", simulater->local_gbcode_.c_str());
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

	// 这里是上面透传下来的数据，已经解析过Base64了
	// 是XML的一部分
	char *message = (char *)pMsg;

	// 加上XML头尾，便于解析
	std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Info>\n";
	xml += message;
	xml += "\n</Info>";

	// 这里要处理一下转UTF8
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
		sprintf_s(msg, 4096, "[%s]收到无法识别的扩展信息：%s", simulater->local_gbcode_.c_str(), xml.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		return SIP_RESPONSE_CODE_BAD_REQUEST;
	}

	tinyxml2::XMLElement *root = doc.RootElement();
	tinyxml2::XMLElement *person_SubCmdType = root->FirstChildElement();
	const char *sub_cmd_type = person_SubCmdType->GetText();

	// 根据协议处理
	if (_stricmp(sub_cmd_type, "BindUser") == 0)
	{
		// 放法1：先回复已收到，后面再重新发
		// 方法2：在后面直接带参回复

		// 先判断密码是否正确
		tinyxml2::XMLElement *element_sub_cmd_result = root->FirstChildElement("SubCmdResult");
		const char *sub_cmd_result = element_sub_cmd_result->GetText();
		if (_stricmp(sub_cmd_result, "FAILED") == 0)
		{
			// 绑定失败了，调用语音接口通报，界面也提示一把
			tinyxml2::XMLElement *element_sub_cmd_result_detail = root->FirstChildElement("SubCmdResultDetail");
			const char *sub_cmd_result_detail = element_sub_cmd_result_detail->GetText();

			simulater->notifer_->RecvBindUser(sub_cmd_result_detail);
			//simulater->speaker_->Speak("账号或密码错误！");
			sprintf_s(msg, 4096, "[%s]收到人机绑定反馈：%s", simulater->local_gbcode_.c_str(), sub_cmd_result_detail);
			std::cout<<msg<<std::endl;
			simulater->app_->logger().information(msg);
			
			return SIP_RESPONSE_CODE_SUCCESS;
		}

		// 发起绑定用户请求，得到平台返回的参数，进行设置，并返回设置成功
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

		// 这里应该是完成了人机绑定的，应当记录一下日志
		sprintf_s(msg, 4096, "[%s]收到人机绑定反馈：绑定成功！", simulater->local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		// 收到绑定用户信息的结果
		// 这里是不是应该扔到一个队列里面处理？

		// 发送成功消息
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
		
		// 塞入队列
		simulater->extra_response_queue_.push(response);
		simulater->wait_queue_not_empty_.set();
	}
	else if (_stricmp(sub_cmd_type, "BindUserConfirm") == 0)
	{
		sprintf_s(msg, 4096, "[%s]收到人机绑定确认信息，此用户之前已绑定了其他设备...", simulater->local_gbcode_.c_str());
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);

		// 接收到用户确认包，这里就不处理了，直接发下一个信令
		simulater->SendBindUserConfirmRecevicedInfo();
		simulater->SendBindUserConfirmInfo();
	}
	else if (_stricmp(sub_cmd_type, "ConfigUpdateReceipt") == 0)
	{
		// 人机绑定回调结果，这里可以不用管
	}
	else if (_stricmp(sub_cmd_type, "DeviceInfo") == 0)
	{
		// 设备基本信息上报结果，这里可以不用管
	}
	else if (_stricmp(sub_cmd_type, "LocationInfo") == 0)
	{
		// 设备定位信息上报结果，这里可以不用管
	}
	else if (_stricmp(sub_cmd_type, "DeviceException") == 0)
	{
		// 设备异常信息上报结果，这里可以不用管
	}
	else if (_stricmp(sub_cmd_type, "RemoteBindUser") == 0)
	{
		// 平台下发绑定用户，这里暂不实现
	}
	else if (_stricmp(sub_cmd_type, "RemoteRecord") == 0)
	{
		// 平台下发远程录像，这里暂不实现
	}
	else if (_stricmp(sub_cmd_type, "RemoteTakePhoto") == 0)
	{
		// 平台下发远程拍照，这里暂不实现
	}
	else if (_stricmp(sub_cmd_type, "RemoteDeviceLock") == 0)
	{
		// 平台下发远程锁定，这里暂不实现
	}
	else if (_stricmp(sub_cmd_type, "RemoteUpdateSetting") == 0)
	{
		// 平台下发更新配置，这里暂不实现
	}
	else if (_stricmp(sub_cmd_type, "EmergencyInfo") == 0)
	{
		// 平台下发警情信息
		tinyxml2::XMLElement *element_emergency_id = root->FirstChildElement("EmergencyId");
		tinyxml2::XMLElement *element_dispatch_time = root->FirstChildElement("DispatchTime");
		tinyxml2::XMLElement *element_dispatch_end_time = root->FirstChildElement("DispatchEndTime");
		
		const char *emergency_id = element_emergency_id->GetText();
		const char *dispatch_time = element_dispatch_time->GetText();
		const char *dispatch_end_time = element_dispatch_end_time->GetText();

		simulater->notifer_->RecvEmergency(emergency_id, dispatch_time, dispatch_end_time);

		// 添加语音播报
		sprintf_s(msg, 4096, "[%s]收到警情信息信息\n警情ID：%s\n处警时间：%s\n处警结束时间%s", simulater->local_gbcode_.c_str(), emergency_id, dispatch_time, dispatch_end_time);
		std::cout<<msg<<std::endl;
		simulater->app_->logger().information(msg);
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}

void GxxGmDSJSimulater::RecvBindUser(const char *result)
{
	//printf("收到用户绑定结果：%s\n", result);
}

void GxxGmDSJSimulater::RecvEmergency(const char *emergency_id, const char *start_time, const char *end_time)
{
	// 没有设置观察者的时候，指向自己，什么都不干
	//printf("收到新警单，警单ID：%s，处警时间：%s，处警结束时间：%s\n", emergency_id, start_time, end_time);
}

void GxxGmDSJSimulater::RecvRemoteBindUser()
{
	// 没有设置观察者的时候，指向自己，什么都不干
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
				// 发送保活心跳
				StruErrorList *error_list = NULL;

				GS28181_ERR err = GS28181_ERR_SUCCESS;
				err = GB28181Agent_HeartBeat(simulater->agent_, &connention_param, 1, NULL);
				if (err != GS28181_ERR_SUCCESS)
				{
					sprintf_s(msg, 4096, "[%s] 发送GB28181保活心跳失败。错误码：%d", simulater->local_gbcode_.c_str(), err);
					std::cout<<msg<<std::endl;
					simulater->app_->logger().error(msg);
				}

				heartbeat_count = 0;
			}

			if (baseinfo_count == simulater->dev_baseinfo_time_)
			{
				// 发送设备基本信息
				simulater->SendBaseInfo();
				baseinfo_count = 0;
			}

			if (location_count == simulater->dev_location_time_)
			{
				// 发送定位信息
				if (simulater->is_standard_gb28181_mobile_position_)
				{
					// 发送标准GB28181-2016定位信息
					simulater->SendLocationInfoEx();
				}
				else
				{
					// 发送扩展定位信息
					simulater->SendLocationInfo();
				}
				location_count = 0;
			}

			if (userbind_count == simulater->dev_userbind_time_)
			{
				simulater->SendBindUserInfo(simulater->police_id_.c_str(), simulater->police_password_.c_str());
				userbind_count = 0;
			}

			// 以1毫秒计数
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

	// 调用接口，发送透传信息
	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, simulator->server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, simulator->server_gbcode_.c_str());
	connention_param.iPort = atoi(simulator->server_port_.c_str());

	while (true)
	{
		// 通过事件检查队列是否为空
		if (simulator->extra_response_queue_.empty())
		{
			//simulator->wait_queue_not_empty_.wait(10);
			Poco::Thread::sleep(5);
			continue;
		}

		ExtraDataResponseInfo extra_data = simulator->extra_response_queue_.front();
		simulator->extra_response_queue_.pop();

		// 这里是否要考虑一下编码问题
		GS28181_ERR err = GB28181Agent_NotifyTransData(simulator->agent_, &connention_param, simulator->local_gbcode_.c_str(), extra_data.extra_msg_.c_str(), extra_data.extra_msg_.size());
		if (err != GS28181_ERR_SUCCESS)
		{
			sprintf_s(msg, 4096, "[%s]发送请求失败！错误码：%d，请求信息：\n%s\n", simulator->local_gbcode_.c_str(), err, extra_data.extra_msg_.c_str());
			std::cout<<msg<<std::endl;
			simulator->app_->logger().error(msg);
		}
		else
		{
			sprintf_s(msg, 4096, "[%s]发送请求成功！错误码：%d，请求信息：\n%s\n", simulator->local_gbcode_.c_str(), err, extra_data.extra_msg_.c_str());
			std::cout<<msg<<std::endl;
			simulator->app_->logger().information(msg);
		}
	}
	
}