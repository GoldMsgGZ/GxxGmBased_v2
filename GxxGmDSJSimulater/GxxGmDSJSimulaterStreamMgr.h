#ifndef _GxxGmDSJSimulaterStreamMgr_H_
#define _GxxGmDSJSimulaterStreamMgr_H_

#include "GB28181Defs.h"
#include "GSRtpServer.h"
//#include "GxxGmVideoFile.h"
#include <map>

#include "Poco/Thread.h"

//#define POCO_THREAD
#define WIN32_THREAD

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
	int SendRealStream();

public:
	static void GS_RTP_CALLBACK _RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData);

public:
	std::map<int, RtpStreamInfo*> stream_maps_;

public:
	bool is_stream_send_thread_stop_;
	HANDLE stream_send_thread_handle_;

#ifdef POCO_THREAD
	Poco::Thread stream_send_thread_;
	static void StreamSendThreadFunc(void* param);
#endif

#ifdef WIN32_THREAD
	
	static DWORD WINAPI StreamSendThreadFuncEx(LPVOID* param);
#endif

public:
	//GxxGmVideoFile video_file_;

public:
	char current_token_[32];
	int ssrc_;
};

#endif//_GxxGmDSJSimulaterStreamMgr_H_
