#include "GxxGmDSJSimulater.h"
#include <Windows.h>
#include <sstream>

#include "Poco/Base64Encoder.h"


GxxGmDSJSimulater::GxxGmDSJSimulater()
: agent_(NULL)
, is_gb28181_heartbeat_thread_need_exit_(false)
, log_file_handle_(NULL)
{
	// 
}

GxxGmDSJSimulater::~GxxGmDSJSimulater()
{
	// 
}

int GxxGmDSJSimulater::Initialize(const char *local_ip, const char *local_port, const char *local_gbcode, const char *server_ip, const char *server_port, const char *server_gbcode, const char *username, const char *password, int is_manual_port, unsigned short begin_port, unsigned short end_port)
{
	int errCode = 0;

	// 初始化协议栈
	agent_ = GB28181Agent_Init(2, 16, Enum28181Version::eVERSION_2016, 3000);
	if (agent_ == NULL)
	{
		// 
		printf("[%s]初始化28181协议栈失败！\n", local_gbcode);
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

	GS28181_ERR err = GB28181Agent_Start(agent_, local_ip, atoi(local_port), local_gbcode, EnumTransType::eTYPE_UDP);
	if (err != GS28181_ERR_SUCCESS)
	{
		printf("[%s]启动28181协议栈失败！错误码：%d\n", local_gbcode, err);
		GB28181Agent_Uninit(agent_);
		return err;
	}

	StruRegistMsg reg_msg;
	
	reg_msg.iExpires = 86400;	// 1年
	strcpy_s(reg_msg.szUserName, STR_USERNAME_LEN, username);
	strcpy_s(reg_msg.szPassword, STR_PASSWORD_LEN, password);
	strcpy_s(reg_msg.stuCnnParam.szIP, STR_IPADDRESS_LEN, server_ip);
	reg_msg.stuCnnParam.iPort = (unsigned int)atoi(server_port);
	strcpy_s(reg_msg.stuCnnParam.szGBCode, STR_GBCODE_LEN, server_gbcode);
	
	char date_time[4096] = {0};
	err = GB28181Agent_Register(agent_, &reg_msg, date_time);
	if (err != GS28181_ERR_SUCCESS)
	{
		printf("[%s]注册到接入网关失败！错误码：%d\n", local_gbcode, err);
		GB28181Agent_Stop(agent_);
		GB28181Agent_Uninit(agent_);
		return err;
	}

	printf("[%s]注册到接入网关成功！错误码：%d\n", local_gbcode, err);

	// 初始化日志
	SYSTEMTIME st;
	GetLocalTime(&st);
	char current_time[128] = {0};
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	char log_file_name[4096] = {0};
	sprintf_s(log_file_name, 4096, "%s.log", current_time, local_gbcode);

	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string tmp = current_program_path;
	int pos = tmp.find_last_of('\\');

	std::string log_path = tmp.substr(0, pos + 1);
	log_path.append("log");
	CreateDirectoryA(log_path.c_str(), NULL);
	log_path.append("\\");
	log_path.append(local_gbcode);
	CreateDirectoryA(log_path.c_str(), NULL);
	log_path.append("\\");
	log_path.append(log_file_name);

	log_file_handle_ = CreateFileA(log_path.c_str(), GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (log_file_handle_ == INVALID_HANDLE_VALUE)
	{
		errCode = GetLastError();
		printf("[%s]创建日志文件失败！\n", local_gbcode);
	}

	// 保存参数
	local_ip_ = local_ip;
	local_port_ = local_port;
	local_gbcode_ = local_gbcode;
	server_ip_ = server_ip;
	server_port_ = server_port;
	server_gbcode_ = server_gbcode;
	username_ = username;
	password_ = password;

	// 初始化流管理模块
	errCode = stream_mgr_.Initialize(is_manual_port, begin_port, end_port, local_ip);
	if (errCode != 0)
	{
		printf("[%s]初始化推流服务失败！\n", local_gbcode);
	}

	return errCode;
}

void GxxGmDSJSimulater::Destroy()
{
	// 先发送命令，停止心跳发送，并且停止基础信息、定位信息的推送
	is_gb28181_heartbeat_thread_need_exit_ = true;
	gb28181_heartbeat_thread_.join();

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

int GxxGmDSJSimulater::SendBindUserInfo(const char *platform_id, const char *device_imei, const char *user_id, const char *password)
{
	// 按照要求构建字符串
	const char *msg_format = "<SubCmdType>BindUser</SubCmdType> \
							  <DeviceStates> \
									<PlatformID>GM_SHENZHEN</PlatformID> \
									<DeviceIMEI>358F27F145B</DeviceIMEI> \
									<UserID>GM001</UserID> \
									<Password>111111</Password> \
							  </DeviceStates>";

	char msg[4096] = {0};
	sprintf_s(msg, 4096, msg_format, 
		platform_id, device_imei, user_id, password);

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
		printf("[%s]发送设备绑定请求失败！错误码：%d\n", local_gbcode_.c_str(), err);
	}

	return err;
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg, strlen(msg));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
		printf("[%s]发送设备基础信息失败！错误码：%d\n", local_gbcode_.c_str(), err);
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
		// 
		printf("[%s]发送设备定位信息失败！错误码：%d\n", local_gbcode_.c_str(), err);
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
		printf("[%s]发送设备异常信息失败！错误码：%d\n", local_gbcode_.c_str(), err);
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
		printf("[%s]发送设备告警信息失败！错误码：%d\n", local_gbcode_.c_str(), err);
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
		// 
		printf("[%s]发送人脸识别请求失败！错误码：%d\n", local_gbcode_.c_str(), err);
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
		printf("[%s]发送车牌识别请求失败！错误码：%d\n", local_gbcode_.c_str(), err);
	}

	return err;
}

void GxxGmDSJSimulater::_AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	// 这是所有的SIP通信日志，可以考虑写入日志文件
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	// 屏幕输出
	printf("%s\n", szTemp);

	// dbgview输出
	OutputDebugStringA(szTemp);

	// 日志文件输出
	DWORD written = 0;
	WriteFile(simulater->log_file_handle_, szTemp, iLen, &written, NULL);
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData)
{
	// 当这里被查询目录之后，立即启动线程，开始发送28181保活心跳
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	int errCode = 0;
	char dbg_msg[4096] = {0};

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
			printf("[%s]响应设备信息查询失败！错误码：%d\n", simulater->local_gbcode_.c_str(), err);
			return SIP_RESPONSE_CODE_SUCCESS;
		}
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
			printf("[%s]响应设备状态查询失败！错误码：%d\n", simulater->local_gbcode_.c_str(), err);
			return SIP_RESPONSE_CODE_SUCCESS;
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
		//strcpy_s(catlog[0].czAddress, STR_ADDRESS_LEN, "广州市黄埔区开创大道2819号");
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
			printf("[%s]响应设备目录查询失败！错误码：%d\n", simulater->local_gbcode_.c_str(), err);
			return SIP_RESPONSE_CODE_SUCCESS;
		}

		// 查询目录响应成功
		// 开始向上推送设备状态信息和定位信息
		// 使用Poco的多线程框架做吧
		if (!simulater->gb28181_heartbeat_thread_.isRunning())
		{
			simulater->gb28181_heartbeat_thread_.start(simulater->GB28181HeartbeatThreadFun, simulater);
			Sleep(10);
		}
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

	// 先分析输入流信息
	if (eRequest == eSTREAM_REALPLAY)
	{
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
			return SIP_RESPONSE_CODE_FAIL;
		}

		// 启动流
		simulater->stream_mgr_.StartRealStream();
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_ExtendRqeustCallBack(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData)
{
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)pUserData;

	// 这里是上面透传下来的吧

	return SIP_RESPONSE_CODE_SUCCESS;
}

void GxxGmDSJSimulater::GB28181HeartbeatThreadFun(void *param)
{
	// 
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)param;

	StruConnectParam connention_param;
	strcpy_s(connention_param.szIP, STR_IPADDRESS_LEN, simulater->server_ip_.c_str());
	strcpy_s(connention_param.szGBCode, STR_GBCODE_LEN, simulater->server_gbcode_.c_str());
	connention_param.iPort = atoi(simulater->server_port_.c_str());

	int heartbeat_count = 0;
	int baseinfo_count = 0;
	while (!simulater->is_gb28181_heartbeat_thread_need_exit_)
	{
		Sleep(1000);
		++heartbeat_count;
		++baseinfo_count;

		if (heartbeat_count == 30)
		{
			// 发送保活心跳
			StruErrorList *error_list = NULL;
			GS28181_ERR err = GB28181Agent_HeartBeat(simulater->agent_, &connention_param, 1, NULL);
			if (err != GS28181_ERR_SUCCESS)
			{
				printf("[%s]发送28181保活心跳失败！\n", simulater->local_gbcode_.c_str());
			}

			heartbeat_count = 0;
		}

		if (baseinfo_count == 5)
		{
			// 发送设备基本信息
			simulater->SendBaseInfo();

			// 发送定位信息
			simulater->SendLocationInfo();

			baseinfo_count = 0;
		}
	}
}