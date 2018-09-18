#ifndef _GB28181AgentSignalObject_H_
#define _GB28181AgentSignalObject_H_

#include "GB28181SimulateDevice.h"
#include "GB28181AgentSDK.h"
#include <string>

class GB28181AgentSignalObject
{
public:
	GB28181AgentSignalObject();
	~GB28181AgentSignalObject();

public:
	int Start(const char *local_ip, int local_port, const char *local_gbcode);
	void Stop();

public:
	int Register(const char *remote_ip, int remote_port, const char *remote_gbcode, const char *authen_username, const char *authen_password);
	void UnRegister();

public:
	/**
	 * 这里是各种业务回调接口
	 */
	static void _AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData);
	static SIP_REPSOND_CODE _DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData);
	static SIP_REPSOND_CODE _DevControlCB(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData);
	static SIP_REPSOND_CODE _PlayControlCB(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData);
	static SIP_REPSOND_CODE _StreamRequestCB(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest,
		StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData);
	static SIP_REPSOND_CODE _NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _ExtendRqeustCallBack(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData);

public:
	int MakeLocalMediaInfo(STREAM_HANDLE hStream, StruMediaInfo *remote_media_info, StruMediaInfo *local_media_info);

public:
	GB28181SimulateDevice *simulate_device_;

public:
	AGENT_HANDLE handle_;

	std::string local_ip_;
	int local_port_;
	std::string local_gbcode_;

	std::string remote_ip_;
	int remote_port_;
	std::string remote_gbcode_;

	std::string authen_username_;
	std::string authen_password_;

public:
	HANDLE hHeartBeat_;
};

#endif//_GB28181AgentSignalObject_H_
