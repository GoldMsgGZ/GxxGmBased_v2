#ifndef _DevInfoHandler_H_
#define _DevInfoHandler_H_

#include "GB28181Defs.h"

class DeviceCatlogMgr
{
public:
	DeviceCatlogMgr();
	~DeviceCatlogMgr();

public:
	// 初始化设备目录管理模块，缓存设备目录信息
	int Init();
	int Update();
	StruCatalogInfo* ToGB28181Format(int &count);
	void ReleaseInfoBuffer(StruCatalogInfo** info);
};

class DevInfoHandler
{
public:
	static SIP_REPSOND_CODE Handler(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData);
};

#endif//_DevInfoHandler_H_
