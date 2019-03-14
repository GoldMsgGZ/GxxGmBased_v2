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
		int heartbeat_rate, int fileupload_rate, int query_orgs_rate, int query_users_rate, const char *video_path,
		const char *audio_path, const char *image_path, const char *gateway_ip,
		int gateway_port, const char *authkey, const char *domain);

	void Close();

public:
	void SetLogger(Poco::Logger *logger);

public:
	void SetFileUploadInfo(std::string file_domain, std::string dsj_id, int file_size,
		std::string file_type, int file_duration, std::string tag_info, std::string org_code, std::string org_name,
		std::string police_code, std::string police_name, std::string storage_path, std::string local_path, std::string play_path,
		std::string storage_server, std::string thumb);

public:
	// 发送心跳
	int SendHeartBeat();

	// 上报文件信息
	int SendFileInfo();

	// 上报执法仪定位信息
	int SendLocationInfo();

public:
	int GetOrgInfo();
	int GetUserInfo();

private:
	int GetDiskTotalSpace();
	int GetDiskUsedSpace();
	int GetCpuUsage();
	int GetMemoryUsage();

public:
	// 心跳相关
	std::string workstaion_id_;			// 采集站ID（配置文件读取）
	std::string workstaion_ip_;			// 采集站IP（配置文件读取）
	std::string workstaion_org_code_;	// 采集站所属部门编号（心跳返回后自动填充）
	std::string workstaion_org_name_;	// 采集站所属部门名称（心跳返回后自动填充）
	
	int hearbeat_rate_;					// 心跳发送频率（配置文件读取）
	int fileupload_rate_;				// 文件上报信息频率（配置文件读取）
	int locationupload_rate_;			// 定位信息上报频率
	int query_orgs_rate_;				// 组织架构查询频率
	int query_users_rate_;				// 用户信息查询频率

	std::string video_path_;			// 视频文件路径（配置文件读取）
	std::string audio_path_;			// 音频文件路径（配置文件读取）
	std::string image_path_;			// 图片路径（配置文件读取）

	std::string gateway_ip_;		// 采集站接入网关IP
	int gateway_port_;				// 采集站接入网关端口号
	std::string authkey_;			// 授权KEY，随意填
	std::string domain_;			// 域，随意填

	// 上报文件信息相关
	std::string file_domain_;		// 文件所属域
	std::string dsj_id_;			// 执法仪国标ID
	int file_size_;					// 文件大小
	std::string file_type_;			// 文件类型
	int file_duration_;				// 文件时长
	std::string tag_info_;			// 备注信息
	std::string org_code_;			// 单位编号或部门编号
	std::string org_name_;			// 单位名称
	std::string police_code_;		// 警员编号
	std::string police_name_;		// 警员姓名
	std::string storage_path_;		// 存储位置
	std::string local_path_;		// 物理位置，采集站上原文件本机存储路径
	std::string play_path_;			// 播放位置：HTTP访问路径
	std::string storage_server_;	// 存储服务器
	std::string thumb_;				// 采集站缩略图

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
