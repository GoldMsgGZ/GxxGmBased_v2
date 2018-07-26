#include "DevControlHandler.h"
#include "GB28181AgentSDK.h"

SIP_REPSOND_CODE DevControlHandler::Handler(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData)
{
	//////////////////////////////////////////////////////////////////////////
	// 设备控制类型
	switch (stuCmd->eType)
	{
	case EnumDevCtrlType::eCTRL_PZT:
		// 云台控制
		break;
	case EnumDevCtrlType::eCTRL_TELEBOOT:
		// 远程启动
		break;
	case EnumDevCtrlType::eCTRL_RECORD:
		// 录像控制
		break;
	case EnumDevCtrlType::eCTRL_GUARD:
		// 报警布防
		break;
	case EnumDevCtrlType::eCTRL_RESET_ALARM:
		// 报警复位
		break;
	case EnumDevCtrlType::eCTRL_ZOOMIN:
		// 拉框放大
		break;
	case EnumDevCtrlType::eCTRL_ZOOMOUT:
		// 拉框缩小
		break;
	case EnumDevCtrlType::eCTRL_CONFIGSET:
		// 设置配置参数
		break;
	case EnumDevCtrlType::eCTRL_IFRAME:
		// 强制关键帧
		break;
	case EnumDevCtrlType::eCTRL_HOMEPOSITION:
		// 看守控制
		break;
	default:
		break;
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}