#ifndef _GxxGmSSH_H_
#define _GxxGmSSH_H_

#include <string>

class GxxGmSSH
{
public:
	GxxGmSSH();
	~GxxGmSSH();

public:
	int Initialize();
	void Close();

public:
	int Login(const char *ipaddr, const char *username, const char *password);
	void Logout();

public:
	int Exec(const char *command, std::string &result);
};

#endif//_GxxGmSSH_H_
