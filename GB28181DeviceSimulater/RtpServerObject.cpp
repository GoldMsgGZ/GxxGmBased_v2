#include "stdafx.h"
#include "RtpServerObject.h"

RtpServerObject::RtpServerObject()
{

}

RtpServerObject::~RtpServerObject()
{

}

int RtpServerObject::Start(int is_manual_port, unsigned short begin_port, unsigned short end_port, const char *local_ip)
{
	StruRtpPort rtp_port;
	rtp_port.eMode = is_manual_port == 1 ? RTP_PORT_MANUAL : RTP_PORT_AUTO;
	rtp_port.iBeginPort = begin_port;
	rtp_port.iEndPort = end_port;

	GSRTP_ERR err = GSRTPServer_Init(&rtp_port, local_ip, RTP_OVER_UDP, RtpServerObject::_RtpServerEventCallBack, this, 5);
	return err;
}

void RtpServerObject::Stop()
{
	GSRTPServer_Cleanup();
}

void GS_RTP_CALLBACK RtpServerObject::_RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData)
{

}