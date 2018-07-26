#ifndef _LogHandler_H_
#define _LogHandler_H_

#include "GB28181Defs.h"

class LogHandler
{
public:
	static void  Handler(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData);
};

#endif//_LogHandler_H_
