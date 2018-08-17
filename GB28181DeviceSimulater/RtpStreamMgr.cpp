#include "stdafx.h"
#include "RtpStreamMgr.h"
#include <string>

RtpStreamMgr::RtpStreamMgr()
{

}

RtpStreamMgr::~RtpStreamMgr()
{

}

int RtpStreamMgr::Init(const char *local_ip, int is_manual_rtp_port, int rtp_port_begin, int rtp_port_finish)
{
	StruRtpPort rtp_port;
	rtp_port.eMode = is_manual_rtp_port == 1 ? RTP_PORT_MANUAL : RTP_PORT_AUTO;
	rtp_port.iBeginPort = rtp_port_begin;
	rtp_port.iEndPort = rtp_port_finish;

	GSRTP_ERR err = GSRTPServer_Init(&rtp_port, local_ip, RTP_OVER_UDP, RtpStreamMgr::RtpServerEventCb, this, 5);
	if (err != GSRTP_SUCCESS)
	{
		return err;
	}

	return 0;
}

int RtpStreamMgr::SetSourceParam(const char * szToken, unsigned int iSSRC, const char * szDestIP, unsigned short iDestPort, const StruRtpAVSampleRate * ptrAVSampleRate, int iBufferSize)
{
	GSRTP_ERR err = GSRTPServer_SetSourceParam(szToken, iSSRC, szDestIP, iDestPort, ptrAVSampleRate, iBufferSize);
	if (err != GSRTP_SUCCESS)
	{
		return err;
	}

	return 0;
}

void RtpStreamMgr::Reclain(const char * szToken, unsigned int iSSRC)
{
	GSRTPServer_Reclaim(szToken, iSSRC);
}

void RtpStreamMgr::RtpServerEventCb(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void* pUserData)
{
	// 这个应该是服务事件回调，我们在这里组织一下信息，扔给调试输出吧
	std::string event_type;
	switch (eEvent)
	{
	case RTP_EVENT_PACKET:
		event_type = "PACKET";
		break;
	case RTP_EVENT_CONNECT:
		event_type = "CONNECT";
		break;
	case RTP_EVENT_DISCONNECT:
		event_type = "";
		break;
	case RTP_EVENT_LOST_PACKET:
		event_type = "";
		break;
	case RTP_EVENT_INVALID_PACKET:
		event_type = "";
		break;
	case RTP_EVENT_OVERFLOW:
		event_type = "";
		break;
	case RTP_EVENT_RTCP_TIMEOUT:
		event_type = "RTCP_TIMEOUT";
		break;
	case RTP_EVENT_RTCP_BYE :
		event_type = "RTCP_BYE";
		break;
	default:
		event_type = "UNKNOW";
		break;
	}

	char msg[8192] = {0};
	sprintf_s(msg, 8192, "[RtpServerEvent] Token:%s, SSRC:%d, event:%s", szToken, iSSRC, event_type.c_str());
	OutputDebugStringA(msg);
}

RtpStreamMgr rtp_stream_mgr_;