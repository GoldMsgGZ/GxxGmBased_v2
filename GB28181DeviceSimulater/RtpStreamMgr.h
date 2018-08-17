#ifndef _RtpPusher_H_
#define _RtpPusher_H_

#include "GSRtpServer.h"

class RtpStreamMgr
{
public:
	RtpStreamMgr();
	~RtpStreamMgr();

public:
	int Init(const char *local_ip, int is_manual_rtp_port, int rtp_port_begin, int rtp_port_finish);
	int SetSourceParam(const char * szToken, unsigned int iSSRC, const char * szDestIP, unsigned short iDestPort, const StruRtpAVSampleRate * ptrAVSampleRate, int iBufferSize);
	void Reclain(const char * szToken, unsigned int iSSRC);

public:
	static void RtpServerEventCb( const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void* pUserData);
};

extern RtpStreamMgr rtp_stream_mgr_;

#endif//_RtpPusher_H_
