#ifndef _GxxGmDocker_H_
#define _GxxGmDocker_H_

#include "GxxGmSSH.h"

class GxxGmDocker
{
public:
	GxxGmDocker();
	~GxxGmDocker();

public:
	/**
	 * 登录到服务器
	 */
	int LoginToServer(const char *ipaddr, const char *username, const char *password);

	/**
	 * 从指定服务器登出
	 */
	void LogoutFromServer(const char *ipaddr);

public:
	/**
	 * 扫描所有镜像列表
	 */
	int ListAllImage();

	/**
	 * 查看所有正在运行的容器
	 */
	int ListAllContainers();

	/**
	 * 查看所有容器占用的资源
	 */
	int ListAllContainerResources();

private:
	std::map<std::string, GxxGmSSH *> servers_;
};

#endif//_GxxGmDocker_H_
