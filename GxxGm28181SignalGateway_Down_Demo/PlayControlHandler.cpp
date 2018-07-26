#include "PlayControlHandler.h"
#include "GB28181AgentSDK.h"

SIP_REPSOND_CODE PlayControlHandler::Handler(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData)
{
	//////////////////////////////////////////////////////////////////////////
	// 根据播放控制类型进行处理
	switch (stuCmd->eType)
	{
	case EnumPlayCtrlType::ePLAY_START:
		// 开始
		break;
	case EnumPlayCtrlType::ePLAY_PAUSE:
		// 暂停
		break;
	case EnumPlayCtrlType::ePLAY_FAST:
		// 快放
		break;
	case EnumPlayCtrlType::ePLAY_SLOW:
		// 慢放
		break;
	case EnumPlayCtrlType::ePLAY_DRAG:
		// 随机拖动
		break;
	default:
		break;
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}