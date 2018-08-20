#include "stdafx.h"
#include "RtpServerObject.h"

RtpStreamMgr::RtpStreamMgr()
{

}

RtpStreamMgr::~RtpStreamMgr()
{

}

int RtpStreamMgr::AddStream(void* stream_handle, int ssrc, enum RtpStreamType stream_type)
{
	RtpStreamInfo *rtp_info = new RtpStreamInfo;
	memset(rtp_info->stream_token_, 0, 32);
	sprintf_s(rtp_info->stream_token_, 32, "%d", stream_handle);

	rtp_info->SSRC_ = ssrc;
	rtp_info->stream_type_ = stream_type;

	int count = stream_maps_.size();
	stream_maps_.insert(std::pair<int, RtpStreamInfo *>(count, rtp_info));

	return 0;
}

RtpStreamInfo* RtpStreamMgr::GetStreamInfo(void* stream_handle)
{
	char current_stream[32] = {0};
	memset(current_stream, 0, 32);
	sprintf_s(current_stream, 32, "%d", stream_handle);

	RtpStreamInfo *element = NULL;

	std::map<int, RtpStreamInfo *>::iterator iter;
	for (iter = stream_maps_.begin(); iter != stream_maps_.end(); ++iter)
	{
		if (memcmp(iter->second->stream_token_, current_stream, 32) == 0)
		{
			element = iter->second;
			break;
		}
	}

	return element;
}

int RtpStreamMgr::RemoveStream(void* stream_handle)
{
	char current_stream[32] = {0};
	memset(current_stream, 0, 32);
	sprintf_s(current_stream, 32, "%d", stream_handle);

	std::map<int, RtpStreamInfo *>::iterator iter;
	for (iter = stream_maps_.begin(); iter != stream_maps_.end(); ++iter)
	{
		if (memcmp(iter->second->stream_token_, current_stream, 32) == 0)
		{
			GSRTPServer_Reclaim(iter->second->stream_token_, iter->second->SSRC_);
			GSRTPServer_RemoveSource(iter->second->stream_token_);
			delete iter->second;
			break;
		}
	}
	
	stream_maps_.erase(iter);
	return 0;
}



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