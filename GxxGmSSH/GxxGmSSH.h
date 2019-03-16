#ifndef _GxxGmSSH_H_
#define _GxxGmSSH_H_

#include <string>
#include "libssh2.h"

#pragma comment(lib, "libssh2.lib")

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
	int Exec(const char *commandline, std::string &result);

private:
	int sock;
	LIBSSH2_SESSION *ssh2_session;
	LIBSSH2_CHANNEL *channel;
};

#endif//_GxxGmSSH_H_
