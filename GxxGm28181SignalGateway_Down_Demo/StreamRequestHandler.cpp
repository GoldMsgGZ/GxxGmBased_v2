#include "StreamRequestHandler.h"
#include "GB28181AgentSDK.h"

SIP_REPSOND_CODE StreamRequestHandler::Handler(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest, StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData)
{
	//////////////////////////////////////////////////////////////////////////
	//
	switch (eRequest)
	{
	case EnumStreamRequest::eSTREAM_REALPLAY:
		// 实时视频播放
		break;
	case EnumStreamRequest::eSTREAM_PLAYBACK:
		// 历史视音频回放
		break;
	case EnumStreamRequest::eSTREAM_DOWNLOAD:
		// 历史视音频下载
		break;
	case EnumStreamRequest::eSTREAM_AUDIO:
		// 语音流请求
		break;
	case EnumStreamRequest::eSTREAM_BYE:
		// 结束流
		break;
	case EnumStreamRequest::eSTREAM_TIMEOUT:
		// 异步流请求超时
		break;
	case EnumStreamRequest::eSTREAM_REJECT:
		// 异步流请求被拒绝
		break;
	default:
		break;
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}