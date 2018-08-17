#include "stdafx.h"
#include "PSPackager.h"

PSPackager::PSPackager()
: handle_(NULL)
{

}

PSPackager::~PSPackager()
{

}

int PSPackager::Open(EnumGSPSCodecType video_codec_id, EnumGSPSCodecType audio_codec_id)
{
	StruESStreamDesc desc;
	desc.eVideoCodecs = video_codec_id;
	desc.eAudioCodecs = audio_codec_id;
	EnumGS_MPEGPS_RetCode err = GS_MPEGPS_CreateES2PSHandle(&handle_, &desc);
	return err;
}

void PSPackager::Close()
{
	EnumGS_MPEGPS_RetCode err = GS_MPEGPS_DestoryES2PSHandle(handle_);
	handle_ = NULL;
}

int PSPackager::Push()
{
	return 0;
}

void CALLBACK PSPackager::_PSDataReceivedCallBack(GS_MpegPSHandle handle, StruPSFrameInfo const* stPSFrameInfo, void* pUserData)
{
	PSPackager *package = (PSPackager*)pUserData;

	// 出PS包了，接下来我们发送出去
}