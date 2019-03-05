#ifndef _GxxGmWSSimulator_H_
#define _GxxGmWSSimulator_H_

#ifdef GXXGMWSSIMULATOR_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <string>

namespace Poco {
	class Thread;
	class Logger;

	namespace Net {
		class HTTPClientSession;
	};
};

class DLL_API GxxGmWSSimulator
{
public:
	GxxGmWSSimulator();
	~GxxGmWSSimulator();

public:
	// 初始化采集工作站
	// 参数：
	//	*ws_id		采集站ID
	//	*authkey	授权KEY
	//	*domain		域名
	int Initialize(const char *ws_id, const char *ws_ip,
		int heartbeat_rate, int fileupload_rate, const char *video_path,
		const char *audio_path, const char *image_path, const char *gateway_ip,
		int gateway_port, const char *authkey, const char *domain);

	void Close();

public:
	void SetLogger(Poco::Logger *logger);

public:
	// 发送心跳
	int SendHeartBeat();

	// 上报文件信息
	int SendFileInfo();

	// 上报定位信息
	int SendLocationInfo();

private:
	int GetDiskTotalSpace();
	int GetDiskUsedSpace();
	int GetCpuUsage();
	int GetMemoryUsage();

public:
	std::string workstaion_id_;
	std::string workstaion_ip_;
	
	int hearbeat_rate_;
	int fileupload_rate_;
	int locationupload_rate_;

	std::string video_path_;
	std::string audio_path_;
	std::string image_path_;

	std::string gateway_ip_;
	int gateway_port_;
	std::string authkey_;
	std::string domain_;

	std::string file_domain_;

public:
	Poco::Logger *logger_;
	Poco::Net::HTTPClientSession *session_;

	Poco::Thread *working_thread_;
	bool is_need_stop_;
	static void WorkingThreadFunc(void* param);
};

#endif//_GxxGmWSSimulator_H_
