#ifndef _GxxGmDSJSimulaterStreamMgr_H_
#define _GxxGmDSJSimulaterStreamMgr_H_

#include "GB28181Defs.h"
#include "GSRtpServer.h"
#include "GxxGmVideoFile.h"
#include <map>

#include "Poco/Thread.h"

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
	int StartRealStream(STREAM_HANDLE streamHandle, int iSSRC, const char *clientIP, int clientPort);

public:
	static void GS_RTP_CALLBACK _RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData);

public:
	std::map<int, RtpStreamInfo*> stream_maps_;

public:
	Poco::Thread stream_send_thread_;
	bool is_stream_send_thread_stop_;
	static void StreamSendThreadFunc(void* param);

public:
	GxxGmVideoFile video_file_;
};

#endif//_GxxGmDSJSimulaterStreamMgr_H_
