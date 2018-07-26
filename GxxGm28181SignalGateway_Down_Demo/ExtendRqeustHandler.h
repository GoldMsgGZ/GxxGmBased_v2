#ifndef _ExtendRqeustHandler_H_
#define _ExtendRqeustHandler_H_

#include "GB28181Defs.h"

class ExtendRqeustHandler
{
public:
	static SIP_REPSOND_CODE Handler(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData);
};

#endif//_ExtendRqeustHandler_H_
