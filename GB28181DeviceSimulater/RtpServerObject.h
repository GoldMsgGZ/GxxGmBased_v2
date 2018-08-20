#ifndef _RtpServerObject_H_
#define _RtpServerObject_H_

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

class RtpStreamMgr
{
public:
	RtpStreamMgr();
	~RtpStreamMgr();

public:
	int AddStream(void* stream_handle, int ssrc, enum RtpStreamType stream_type);
	RtpStreamInfo* GetStreamInfo(void* stream_handle);
	int RemoveStream(void* stream_handle);

public:
	std::map<int, RtpStreamInfo*> stream_maps_;
};

class RtpServerObject
{
public:
	RtpServerObject();
	~RtpServerObject();

public:
	int Start(int is_manual_port, unsigned short begin_port, unsigned short end_port, const char *local_ip);
	void Stop();

public:
	static void GS_RTP_CALLBACK _RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData);
};

#endif
