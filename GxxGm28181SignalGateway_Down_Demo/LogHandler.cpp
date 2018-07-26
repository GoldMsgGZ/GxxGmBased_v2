#include "LogHandler.h"
#include "GB28181AgentSDK.h"

#include <iostream>
#include <string>

void LogHandler::Handler(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData)
{
	std::string info;
	// ¥Ú”°»’÷æ∞…
	switch (eLevel)
	{
	case EnumLogLevel::eLOG_LEVEL_INFO:
		info += "[INFO] ";
		break;
	case EnumLogLevel::eLOG_LEVEL_WARN:
		info += "[WARN] ";
		break;
	case EnumLogLevel::eLOG_LEVEL_ERROR:
		info += "[ERROR] ";
		break;
	case EnumLogLevel::eLOG_LEVEL_FATAL:
		info += "[FATAL] ";
		break;
	case EnumLogLevel::eLOG_LEVEL_DEBUG:
		info += "[DEBUG] ";
		break;
	default:
		break;
	}

	info += szTemp;
	std::cout<<info.c_str()<<std::endl;
}