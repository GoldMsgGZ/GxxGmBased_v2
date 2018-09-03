#ifndef _GB28181ServerSignalObject_H_
#define _GB28181ServerSignalObject_H_

#include "GB28181ServerSDK.h"
#include "GB28181Defs.h"
#include <string>
#include <map>

class GB28181ServerSignalObject
{
public:
	GB28181ServerSignalObject();
	~GB28181ServerSignalObject();

public:
	int Start(const char *srv_ip, int srv_port, const char *srv_gbcode, const char *srv_authen_pin);
	void Stop();

public:
	static SIP_REPSOND_CODE _DevRegist_CallBackFunc(const StruRegistMsg * stuMsg, void * pUserData);
	static void _QueryResult_CallBackFunc(EnumQueryType eType, const char * czAgentGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czAgentGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _StreamRequest_CallBackFunc(STREAM_HANDLE hStream, const char * czAgentGode, EnumStreamRequest eRequest, 
		const StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData);
	static void _ServerLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData);

public:
	//std::map<std::string, struct StruRegistMsg dev_info> registed_devs_;
	
private:
	SERVER_HANDLE handle_;

	std::string srv_ip_;
	int srv_port_;
	std::string srv_gbcode_;
};

#endif//_GB28181ServerSignalObject_H_