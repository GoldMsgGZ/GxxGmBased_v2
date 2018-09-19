#ifndef _GxxGmDSJSimulaterStreamMgr_H_
#define _GxxGmDSJSimulaterStreamMgr_H_

#include "GB28181Defs.h"
#include "GSRtpServer.h"
#include <map>

enum RtpStreamType
{
	RtpStream_Real,
	RtpStream_History
};

struct RtpStreamInfo
{
	char stream_token_[32];
	int SSRC_; 
	enum RtpStreamType stream_type_;
};

class GxxGmDSJSimulaterStreamMgr
{
public:
	GxxGmDSJSimulaterStreamMgr();
	~GxxGmDSJSimulaterStreamMgr();

public:
	int Initialize(int is_manual_port, unsigned short begin_port, unsigned short end_port, const char *local_ip);
	void UnInitialize();

public:
	int AddRealStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort);
	int StartRealStream();

public:
	static _RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData);

public:
	std::map<int, RtpStreamInfo*> stream_maps_;
};

#endif//_GxxGmDSJSimulaterStreamMgr_H_
