#ifndef _PlayControlHandler_H_
#define _PlayControlHandler_H_

#include "GB28181Defs.h"

class PlayControlHandler
{
public:
	static SIP_REPSOND_CODE Handler(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData);
};

#endif//_PlayControlHandler_H_
