#include "GxxGmDSJSimulater.h"

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
	// 
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

void GxxGmDSJSimulater::SetExceptionInfo(DEVICE_EXCEPTION_INFO exception_info)
{
	// 
	exception_info_.battery_	= exception_info_.battery_;
	exception_info_.ccd_		= exception_info_.ccd_;
	exception_info_.storage_	= exception_info_.storage_;
	exception_info_.mic_		= exception_info_.mic_;
}

int GxxGmDSJSimulater::SendBindUserInfo()
{
	int errCode = 0;

	// 按照要求构建字符串
	//const char *msg_format = "<SubCmdType>DeviceInfo</SubCmdType> \
	//	<DeviceStates> \
	//	<Carrieroperator>%s</Carrieroperator> \
	//	<Nettype>%s</Nettype> \
	//	<Signal>%s</Signal> \
	//	<Battery>%s</Battery> \
	//	<Storage>%s</Storage> \
	//	<CPU>%s</CPU> \
	//	<Version>%s</Version> \
	//	<LocalRecord>%s</LocalRecord> \
	//	</DeviceStates>";

	//char msg[4096] = {0};
	//sprintf_s(msg, 4096, msg_format, );

	return errCode;
}

int GxxGmDSJSimulater::SendBaseInfo()
{
	int errCode = 0;

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

	GS28181_ERR err = GB28181Agent_NotifyTransData(agent_, &connention_param, local_gbcode_.c_str, msg_format, strlen(msg_format));

	return errCode;
}

int GxxGmDSJSimulater::SendLocationInfo()
{
	int errCode = 0;

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

	return errCode;
}

int GxxGmDSJSimulater::SendExceptionInfo()
{
	int errCode = 0;

	const char *msg_format = "<SubCmdType>DeviceException</SubCmdType> \
							<Exceptions> \
							<Storage>NotFound</Storage> \
							<Battery>Removed</Battery> \
							<CCD>Error</CCD> \
							<MIC>Error</MIC> \
							</Exceptions>";

	return errCode;
}

int GxxGmDSJSimulater::SendAlarmInfo()
{
	int errCode = 0;

	const char *msg_format = "<SubCmdType>DeviceSOS</SubCmdType> \
							<DeviceSOSInfo> \
							<Datetime>2018-05-22 18:03:01</Datetime> \
							<Location>  \
							<DivisionNS>N/S</DivisionNS> \
							<DivisionEW>E/W</DivisionEW> \
							<Radius>3</Radius > \
							<Longitude>12345678</Longitude> \
							<Latitude>23456789</Latitude> \
							<Direction>0</Direction> \
							<Speed>3</Speed> \
							<Height>3</Height> \
							<LocationTime>2018-05-22 18:03:01.876</LocationTime> \
							<Satellites>3</Satellites> \
							<LocationAvailable>1</LocationAvailable> \
							</Location> \
							</DeviceSOSInfo>";

	return errCode;
}

int GxxGmDSJSimulater::SendFaceInfo()
{
	int errCode = 0;

	const char *msg_format = "<SubCmdType>FaceRecognition</SubCmdType> \
							<FaceInfo> \
							<FaceImageData>1A2B3C4D5F1A2B3C4D5F…</FaceImageData> \
							<DateTime>2018-08-15 18:15:07</DateTime> \
							</FaceInfo>";

	return errCode;
}

int GxxGmDSJSimulater::SendCarIdInfo()
{
	int errCode = 0;

	const char *msg_format = "<SubCmdType>CarIDRecognition</SubCmdType> \
							<CarIDInfo> \
							<CarIDImageData>1A2B3C4D5F1A2B3C4D5F…</CarIDImageData> \
							<DateTime>2018-08-15 18:15:07</DateTime> \
							</CarIDInfo>";

	return errCode;
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
