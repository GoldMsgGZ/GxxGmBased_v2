#ifndef _DBBusiness_H_
#define _DBBusiness_H_

#include "DataStruct.h"
#include "sqlite3.h"


class DBBusiness
{
public:
	DBBusiness();
	~DBBusiness();

public:
	std::string GetCurrentIp();
	std::string GetCurrentPort();
	std::string GetCurrentGbcode();

	std::string GetRemoteIp();
	std::string GetRemotePort();
	std::string GetRemoteGbcode();
	std::string GetAuthenUsername();
	std::string GetAuthenPassword();

public:
	int SetCurrentIp(const char *current_ip);
	int SetCurrentPort(const char *current_port);
	int SetCurrentGbcode(const char *current_gbcode);

	int SetRemoteIp(const char *remote_ip);
	int SetRemotePort(const char *remote_port);
	int SetRemoteGbcode(const char *remote_gbcode);
	int SetAuthenUsername(const char *authen_username);
	int SetAuthenPassword(const char *authen_password);

public:
	int GetOrgInfos(struct OrgInfo &infos);

private:
	// 逆天的递归函数，用于从根节点开始构建组织架构树
	int BuildOrgTree(struct OrgInfo *root_info, OrgInfo current_info, int level);

private:
	sqlite3 *db_;
};

#endif//_DBBusiness_H_
