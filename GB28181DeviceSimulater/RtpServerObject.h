#ifndef _RtpServerObject_H_
#define _RtpServerObject_H_

#include "GSRtpServer.h"

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
