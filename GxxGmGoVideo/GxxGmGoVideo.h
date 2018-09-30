#ifndef _GxxGmGoVideo_H_
#define _GxxGmGoVideo_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>

typedef struct _GOVIDEO_DEVICE_INFO_
{
	unsigned int device_id_;
	unsigned int device_index_;
	std::string device_name_;
	unsigned int model_id_;
	unsigned int category_id_;
	std::string device_code_;
	std::string device_connection_info_;
	std::string device_version_;
	std::string device_username_;
	std::string device_password_;
	std::string device_extra_info_;
	std::string gb28181_code_;
	std::string device_name_abbr_;
	unsigned int version_;
	std::string remark_;
	unsigned int dgw_server_id_;

} GOVIDEO_DEVICE_INFO, *PGOVIDEO_DEVICE_INFO;

typedef struct _GOVIDEO_DEVICE_STATUS_
{
	std::string gb28181_code_;
	unsigned int device_id_;
	unsigned int status_;
	std::map<int, unsigned int> chennal_status_;
} GOVIDEO_DEVICE_STATUS, *PGOVIDEO_DEVICE_STATUS;

class GxxGmGoVideo
{
public:
	GxxGmGoVideo();
	~GxxGmGoVideo();

public:
	int Login(const char *govideo_ip, unsigned short govideo_port, const char *sequence_id = "admin", const char *login_id = "admin", int login_type = 107, const char *username = "admin", const char *password = "admin");
	int Logout();

public:
	
	// 查询所有设备(通道)
	int GetAllDevices();

	// 查询设备状态，会得到设备在线状态，设备通道列表以及通道在线状态
	int GetDeviceStatus(const char *device_gb28181_code, GOVIDEO_DEVICE_STATUS &device_status);

	// 查询所有设备状态
	int GetAllDeviceStatus();

	// 接收GoVideo发来的设备变更信息

	// 点流
	int GetRealStream(unsigned int device_id, std::string &stream_url);
	int GetRealStreamByGBCode(const char *device_gb28181_code, std::string &stream_url);

	// 语音对讲
	int StartTalk(const char *device_id, std::string &stream_url);

private:
	void* http_session_;
	std::string token_;

public:
	std::vector<GOVIDEO_DEVICE_INFO *> devices_;
	std::vector<GOVIDEO_DEVICE_STATUS> devices_status_;

private:
	std::string host_;
	unsigned short port_;
};

#endif//_GxxGmGoVideo_H_
