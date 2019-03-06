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

	// 上报执法仪定位信息
	int SendLocationInfo();

private:
	int GetDiskTotalSpace();
	int GetDiskUsedSpace();
	int GetCpuUsage();
	int GetMemoryUsage();

public:
	// 心跳相关
	std::string workstaion_id_;		// 采集站ID
	std::string workstaion_ip_;		// 采集站IP
	
	int hearbeat_rate_;				// 心跳发送频率
	int fileupload_rate_;			// 文件上报信息频率
	int locationupload_rate_;		// 定位信息上报频率

	std::string video_path_;		// 视频文件路径
	std::string audio_path_;		// 音频文件路径
	std::string image_path_;		// 图片路径

	std::string gateway_ip_;		// 采集站接入网关IP
	int gateway_port_;				// 采集站接入网关端口号
	std::string authkey_;			// 授权KEY，随意填
	std::string domain_;			// 域，随意填

	// 上报文件信息相关
	std::string file_domain_;		// 文件所属域
	std::string dsj_id_;			// 执法仪国标ID

	// 执法仪定位信息
	std::string latitude_;
	std::string longtitude_;

public:
	Poco::Logger *logger_;
	Poco::Net::HTTPClientSession *session_;

	Poco::Thread *working_thread_;
	bool is_need_stop_;
	static void WorkingThreadFunc(void* param);
};

#endif//_GxxGmWSSimulator_H_
