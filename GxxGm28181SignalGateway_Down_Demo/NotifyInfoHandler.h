#ifndef _NotifyInfoHandler_H_
#define _NotifyInfoHandler_H_

#include "GB28181Defs.h"

class NotifyInfoHandler
{
public:
	static SIP_REPSOND_CODE Handler(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData);
};

#endif//_NotifyInfoHandler_H_
