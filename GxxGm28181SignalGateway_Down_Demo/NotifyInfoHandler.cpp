#include "NotifyInfoHandler.h"
#include "GB28181AgentSDK.h"

SIP_REPSOND_CODE NotifyInfoHandler::Handler(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData)
{
	switch (eType)
	{
		//////////////////////////////////////////////////////////////////////////
		//服务端接收到的通知
	case EnumNotifyType::eNOTIFY_HEARTBEAT:
		// 心跳通知
		break;
	case EnumNotifyType::eNOTIFY_MEDIASTATUS:
		// 媒体状态
		break;
	case EnumNotifyType::eNOTIFY_ALARM:
		// 报警
		break;
	case EnumNotifyType::eNOTIFY_ALARMFORSUBS:
		// 针对订阅而发送的报警
		break;
	case EnumNotifyType::eNOTIFY_CATALOG:
		// 目录通知
		break;
	case EnumNotifyType::eNOTIFY_MOBILEPOS:
		// 移动设备位置信息通知
		break;
	case EnumNotifyType::eNOTIFY_TRANSDATA:
		// 代理端发来的透传数据
		break;

		//////////////////////////////////////////////////////////////////////////
		//代理端接收到的通知
	case EnumNotifyType::eNOTIFY_BROADCAST:
		// 语音广播
		break;
	case EnumNotifyType::eNOTIFY_CATASUBS:
		// 收到目录订阅通知
		break;
	case EnumNotifyType::eNOTIFY_ALARMSUBS:
		// 收到报警订阅通知
		break;
	case EnumNotifyType::eNOTIFY_MOBILEPOSSUB:
		// 收到移动设备位置订阅通知
		break;

		//////////////////////////////////////////////////////////////////////////
		//协议内部通知消息
	case EnumNotifyType::eNOTIFY_SUBSEXPIRED:
		// 订阅中止
		break;
	default:
		break;
	}
	return SIP_RESPONSE_CODE_SUCCESS;
}