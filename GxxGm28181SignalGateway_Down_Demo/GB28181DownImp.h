#ifndef _GB28181DownImp_H_
#define _GB28181DownImp_H_

#include "GB28181AgentSDK.h"

class DeviceCatlogMgr;

class GB28181DownImp
{
public:
	GB28181DownImp();
	~GB28181DownImp();

public:
	int Init(const char *local_ip, int local_port, const char *local_gbcode, const char *remote_ip, int remote_port, const char *remote_gbcode, const char *username, const char *password);
	void Close();

public:
	void Run();

public:
	// 组织架构与设备列表管理器
	DeviceCatlogMgr *device_catlog_mgr_;

private:
	AGENT_HANDLE handler_;
};

#endif//_GB28181DownImp_H_
