#include "GxxGmDSJSimulaterStreamMgr.h"

GxxGmDSJSimulaterStreamMgr::GxxGmDSJSimulaterStreamMgr()
{
	// 
}

GxxGmDSJSimulaterStreamMgr::~GxxGmDSJSimulaterStreamMgr()
{
	// 
}

int GxxGmDSJSimulaterStreamMgr::Initialize(int is_manual_port, unsigned short begin_port, unsigned short end_port, const char *local_ip)
{
	StruRtpPort rtp_port;
	rtp_port.eMode = is_manual_port == 1 ? RTP_PORT_MANUAL : RTP_PORT_AUTO;
	rtp_port.iBeginPort = begin_port;
	rtp_port.iEndPort = end_port;

	GSRTP_ERR err = GSRTPServer_Init(&rtp_port, local_ip, RTP_OVER_UDP, GxxGmDSJSimulaterStreamMgr::_RtpServerEventCallBack, this, 5);
	return err;
}

void GxxGmDSJSimulaterStreamMgr::UnInitialize()
{
	GSRTPServer_Cleanup();
}

int GxxGmDSJSimulaterStreamMgr::AddRealStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort)
{
	int errCode = 0;

	// Ê×ÏÈ¼ÆËãToken
	char current_token[32] = {0};
	memset(current_token, 0, 32);
	sprintf_s(current_token, 32, "%d", streamHandle);

	errCode = GSRTPServer_AddSource(current_token, iSSRC, &iLocalPort);
	if (errCode == GSRTP_SUCCESS)
	{
		RtpStreamInfo *rtp_info = new RtpStreamInfo;
		memset(rtp_info->stream_token_, 0, 32);
		sprintf_s(rtp_info->stream_token_, 32, "%d", streamHandle);

		rtp_info->SSRC_ = iSSRC;
		rtp_info->stream_type_ = RtpStream_Real;

		int count = stream_maps_.size();
		stream_maps_.insert(std::pair<int, RtpStreamInfo *>(count, rtp_info));
	}

	return errCode;
}

int GxxGmDSJSimulaterStreamMgr::StartRealStream()
{

}