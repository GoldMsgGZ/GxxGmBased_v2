#include "GxxGmDSJSimulater.h"
#include <Windows.h>
#include <sstream>

#include "Poco/Base64Encoder.h"
#include "Poco/Thread.h"

GxxGmDSJSimulater::GxxGmDSJSimulater()
: agent_(NULL)
{
	// 
}

GxxGmDSJSimulater::~GxxGmDSJSimulater()
{
	// 
}

int GxxGmDSJSimulater::Initialize(const char *local_ip, const char *local_port, const char *local_gbcode, const char *server_ip, const char *server_port, const char *server_gbcode, const char *username, const char *password)
{
	int errCode = 0;

	// 初始化协议栈
	agent_ = GB28181Agent_Init(2, 16, Enum28181Version::eVERSION_2016, 3000);
	if (agent_ == NULL)
	{
		// 
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
		GB28181Agent_Uninit(agent_);
		return err;
	}

	StruRegistMsg reg_msg;
	memset(&reg_msg, 0, sizeof(StruRegistMsg));

	reg_msg.iExpires = 31536000;	// 1年
	strcpy_s(reg_msg.szUserName, 64, username);
	strcpy_s(reg_msg.szPassword, 64, password);
	strcpy_s(reg_msg.stuCnnParam.szIP, 64, server_ip);
	reg_msg.stuCnnParam.iPort = atoi(server_port);
	strcpy_s(reg_msg.stuCnnParam.szGBCode, 64, server_gbcode);
	
	char date_time[4096] = {0};
	err = GB28181Agent_Register(agent_, &reg_msg, date_time);
	if (err != GS28181_ERR_SUCCESS)
	{
		GB28181Agent_Stop(agent_);
		GB28181Agent_Uninit(agent_);
		return err;
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

	return 0;
}

void GxxGmDSJSimulater::Destroy()
{
	// 先发送命令，停止心跳发送，并且停止基础信息、定位信息的推送

	GB28181Agent_Stop(agent_);
	GB28181Agent_Uninit(agent_);
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
	base_info.version_			= base_info.version_;
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
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
	sprintf_s(current_time, 128, "%d-%02d-%02d %02d-%02d-%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
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
	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str(), msg_format, strlen(msg_format));
	if (err != GS28181_ERR_SUCCESS)
	{
		// 
	}

	return err;
}

void GxxGmDSJSimulater::_AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	// 这是所有的SIP通信日志，可以考虑写入日志文件
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
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_STATUS)
	{
		// 设备状态查询
	}
	else if (stuQuery->eType == EnumQueryType::eQUE_DEV_CATALOG)
	{
		// 设备目录查询
		StruDeviceCatalog stuCata;
		strcpy_s(stuCata.czGBCode, STR_GBCODE_LEN, stuQuery->czGBCode);

		stuCata.iSumNum = 1;
		stuCata.ptrCatalog = new StruCatalogInfo;
		strcpy_s(stuCata.ptrCatalog->czAddress, STR_ADDRESS_LEN, "广州市黄埔区开创大道2819号");
		strcpy_s(stuCata.ptrCatalog->czBlock, STR_BLOCK_LEN, "联合街道暹岗警区");
		strcpy_s(stuCata.ptrCatalog->czCivilCode, STR_CIVILCODE_LEN, "440100");
		strcpy_s(stuCata.ptrCatalog->czDeviceName, STR_NAME_LEN, "高新兴国迈G4执法记录仪");
		strcpy_s(stuCata.ptrCatalog->czGBCode, STR_GBCODE_LEN, simulater->local_gbcode_.c_str());
		strcpy_s(stuCata.ptrCatalog->czIPAddress, STR_IPADDRESS_LEN, simulater->local_ip_.c_str());
		strcpy_s(stuCata.ptrCatalog->czManufacturer, STR_MANUFACTURER_LEN, "高新兴国迈科技有限公司");
		strcpy_s(stuCata.ptrCatalog->czModel, STR_MODEL_LEN, "DSJ-G4");
		strcpy_s(stuCata.ptrCatalog->czOwner, STR_OWNER_LEN, "440000");
		strcpy_s(stuCata.ptrCatalog->czParentID, STR_PARENTID_LEN, simulater->local_gbcode_.c_str());
		stuCata.ptrCatalog->dLongitude = 0;
		stuCata.ptrCatalog->dLatitude = 0;
		stuCata.ptrCatalog->iParental = 0;
		stuCata.ptrCatalog->iPort = atoi(simulater->local_port_.c_str());
		stuCata.ptrCatalog->iRegisterWay = 1;
		stuCata.ptrCatalog->iSafetyWay = 0;
		stuCata.ptrCatalog->iSecrecy = 0;
		stuCata.ptrCatalog->iStatus = 1;

		GS28181_ERR err = GB28181Agent_RespondDevCatalog(hSession, &stuCata, 0, false);
		if(GS28181_ERR_SUCCESS == err)
		{
			// 查询目录响应失败
		}

		// 查询目录响应成功
		// 开始向上推送设备状态信息和定位信息
		// 使用Poco的多线程框架做吧
		gb28181_heartbeat_thread_.start(simulater->GB28181HeartbeatThreadFun, simulater);
		Sleep(10);
		gb28181_extend_info_thread_.start(simulater->GB28181ExtendInfoThreadFun, simulater);
		Sleep(10);
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

	return SIP_RESPONSE_CODE_SUCCESS;
}

void GxxGmDSJSimulater::GB28181HeartbeatThreadFun(void *param)
{
	// 
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)param;

	while (simulater)
}

void GxxGmDSJSimulater::GB28181ExtendInfoThreadFun(void *param)
{
	// 
	GxxGmDSJSimulater *simulater = (GxxGmDSJSimulater *)param;
}