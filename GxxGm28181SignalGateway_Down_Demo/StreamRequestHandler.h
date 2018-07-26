#ifndef _StreamRequestHandler_H_
#define _StreamRequestHandler_H_

#include "GB28181Defs.h"

class StreamRequestHandler
{
public:
	static SIP_REPSOND_CODE Handler(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest, StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData);
};

#endif//_StreamRequestHandler_H_
