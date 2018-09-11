#include "GxxGmDSJSimulater.h"

GxxGmDSJSimulater::GxxGmDSJSimulater()
{
	// 
}

GxxGmDSJSimulater::~GxxGmDSJSimulater()
{
	// 
}

int GxxGmDSJSimulater::Initialize(const char *local_ip, const char *local_port, const char *local_gbcode)
{
	int errCode = 0;

	return errCode;
}

void GxxGmDSJSimulater::Destroy()
{
	// 
}

void GxxGmDSJSimulater::SetBaseInfo(DEVICE_BASE_INFO base_info)
{
	// 
}

void GxxGmDSJSimulater::SetLocationInfo(DEVICE_LOCATION_INFO location_info)
{
	// 
}

void GxxGmDSJSimulater::SetExceptionInfo(DEVICE_EXCEPTION_INFO exception_info)
{
	// 
}

int GxxGmDSJSimulater::SendBindUserInfo()
{
	int errCode = 0;

	return errCode;
}

int GxxGmDSJSimulater::SendBaseInfo()
{
	int errCode = 0;

	return errCode;
}

int GxxGmDSJSimulater::SendLocationInfo()
{
	int errCode = 0;

	return errCode;
}

int GxxGmDSJSimulater::SendExceptionInfo()
{
	int errCode = 0;

	return errCode;
}

int GxxGmDSJSimulater::SendAlarmInfo()
{
	int errCode = 0;

	return errCode;
}

int GxxGmDSJSimulater::SendFaceInfo()
{
	int errCode = 0;

	return errCode;
}

int GxxGmDSJSimulater::SendCarIdInfo()
{
	int errCode = 0;

	return errCode;
}

void GxxGmDSJSimulater::_AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	// 
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData)
{
	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_DevControlCB(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData)
{
	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_PlayControlCB(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData)
{
	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_StreamRequestCB(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest, StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData)
{
	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData)
{
	return SIP_RESPONSE_CODE_SUCCESS;
}

SIP_REPSOND_CODE GxxGmDSJSimulater::_ExtendRqeustCallBack(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData)
{
	return SIP_RESPONSE_CODE_SUCCESS;
}
