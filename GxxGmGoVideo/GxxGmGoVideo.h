#ifndef _GxxGmGoVideo_H_
#define _GxxGmGoVideo_H_

#include <string>
#include <sstream>

class GxxGmGoVideo
{
public:
	GxxGmGoVideo();
	~GxxGmGoVideo();

public:
	int Login(const char *govideo_ip, unsigned short govideo_port, const char *sequence_id = "admin", const char *login_id = "admin", int login_type = 107, const char *username = "admin", const char *password = "admin");
	int Logout();

public:
	// 点流指令
	int GetStream();
	// 停留指令
	int 

private:
	std::string token_;

private:
	std::string host_;
	unsigned short port_;
};

#endif//_GxxGmGoVideo_H_
