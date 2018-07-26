#include "ExtendRqeustHandler.h"
#include "GB28181AgentSDK.h"

SIP_REPSOND_CODE ExtendRqeustHandler::Handler(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData)
{
	switch (eType)
	{
	case EnumExtendType::eEX_TRANSDATA:
		// 透传请求，收到透传请求后直接扔到队列中
		break;
	default:
		break;
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}