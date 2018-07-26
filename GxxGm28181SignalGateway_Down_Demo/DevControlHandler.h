#ifndef _DevControlHandler_H_
#define _DevControlHandler_H_

#include "GB28181Defs.h"

class DevControlHandler
{
public:
	static SIP_REPSOND_CODE Handler(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData);
};

#endif//_DevControlHandler_H_
