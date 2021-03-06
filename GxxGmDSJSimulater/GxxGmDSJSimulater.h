#ifndef _GxxGmDSJSimulater_H_
#define _GxxGmDSJSimulater_H_

#include "GxxGmDeviceInfo.h"
#include "GxxGmDSJSimulaterStreamMgr.h"
#include "GB28181AgentSDK.h"
#include <string>
#include <queue>

#include "Poco/Thread.h"
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/AutoPtr.h"
#include "Poco/Util/Application.h"

class GxxGmWordSpeaker;

#ifdef GXXGMDSJSIMULATER_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

struct SimulaterInitInfo
{
	std::string local_ip_;
	std::string local_port_;
	std::string local_gbcode_;

	std::string server_ip_;
	std::string server_port_;
	std::string server_gbcode_;
	std::string username_;
	std::string password_;

	int manual_port_;
	unsigned short begin_port_;
	unsigned short end_port_;

	std::string sip_net_;
	std::string rtp_net_;
	std::string stream_file_;
	int gb28181_hb_time_;
	int dev_baseinfo_time_;
	int dev_location_time_;
	int dev_userbind_time_;
	int dev_exception_time_;

	int start_dev_userbind_;
	std::string police_number_;
	std::string police_password_;

	std::string imei_;
	std::string platform_id_;
};

struct ExtraDataResponseInfo
{
	int send_type_;
	std::string target_device_id_;
	std::string extra_msg_;
};

class DLL_API GxxGmDSJSimulaterNotifer
{
public:
	// 收到设备主动绑定回复
	virtual void RecvBindUser(const char *result) = 0;

	// 收到远程绑定用户请求
	virtual void RecvRemoteBindUser() = 0;

	// 收到警情
	virtual void RecvEmergency(const char *emergency_id, const char *start_time, const char *end_time) = 0;
};

class DLL_API GxxGmDSJSimulater : public GxxGmDSJSimulaterNotifer
{
public:
	GxxGmDSJSimulater();
	~GxxGmDSJSimulater();

public:
	// 开机
	int Initialize(struct SimulaterInitInfo &init_info, FFMpegStub *ffmpeg_stub, Poco::Util::Application *app);

	// 关机
	void Destroy();

	void SetNotifer(GxxGmDSJSimulaterNotifer *notifer);

public:
	int Register();

public:
	// 设置设备基本信息
	void SetBaseInfo(DEVICE_BASE_INFO base_info);
	// 设置设备定位信息
	void SetLocationInfo(DEVICE_LOCATION_INFO location_info);
	// 设置设备异常信息
	void SetExceptionInfo(DEVICE_EXCEPTION_INFO excep_info);

public:
	// 发送绑定用户请求
	int SendBindUserInfo(const char *user_id, const char *password);
	int SendBindUserConfirmRecevicedInfo();
	int SendBindUserConfirmInfo();
	// 发送设备基本信息
	int SendBaseInfo();
	// 发送设备定位信息
	int SendLocationInfo();
	int SendLocationInfoEx();
	int UpdateLocationPos();
	// 发送设备异常信息
	int SendExceptionInfo();
	// 发送设备告警信息
	int SendAlarmInfo();
	// 发送人脸识别信息
	int SendFaceInfo(const char *face_img, int face_img_len);
	// 发送车牌识别信息
	int SendCarIdInfo();

public:
	static void _AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData);
	static SIP_REPSOND_CODE _DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData);
	static SIP_REPSOND_CODE _DevControlCB(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData);
	static SIP_REPSOND_CODE _PlayControlCB(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData);
	static SIP_REPSOND_CODE _StreamRequestCB(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest, StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData);
	static SIP_REPSOND_CODE _NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _ExtendRqeustCallBack(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData);

public:
	// 收到设备主动绑定回复
	virtual void RecvBindUser(const char *result);
	// 收到远程绑定用户请求
	virtual void RecvRemoteBindUser();
	// 收到警情
	virtual void RecvEmergency(const char *emergency_id, const char *start_time, const char *end_time);

public:
	static void GB28181HeartbeatThreadFun(void *param);
	Poco::Thread gb28181_heartbeat_thread_;
	bool is_gb28181_heartbeat_thread_need_exit_;

	static void PositionThreadFun(void *param);
	Poco::Thread position_thread_;
	bool is_standard_gb28181_mobile_position_;

public:
	StruRegistMsg reg_msg;

public:
	std::string local_ip_;			// 执法仪IP
	std::string local_port_;		// 执法仪端口号
	std::string local_gbcode_;		// 执法仪国标码
	std::string server_ip_;			// 接入网关IP
	std::string server_port_;		// 接入端口
	std::string server_gbcode_;		// 接入国标码
	std::string username_;			// 鉴权用户名
	std::string password_;			// 鉴权密码

	AGENT_HANDLE agent_;

public:
	// 简单粗暴一点，文档日志就直接普普通通的写文档
	HANDLE log_file_handle_;

public:
	int mobile_position_sub_id_;
	int gb28181_hb_time_;		// 保活心跳间隔时间
	int dev_baseinfo_time_;		// 基础信息间隔时间
	int dev_location_time_;		// 定位信息间隔时间
	int dev_userbind_time_;		// 人机绑定信息间隔
	int dev_exception_time_;	// 设备异常信息间隔

	std::string imei_;			// 模拟器IMEI
	std::string platform_id_;

	DEVICE_BASE_INFO base_info_;
	DEVICE_LOCATION_INFO location_info_;
	DEVICE_EXCEPTION_INFO exception_info_;

	double last_longtitude_;	// 用于计算GPS坐标推移
	double last_latitude_;		// 用于计算GPS坐标推移

public:
	int start_dev_userbind_;
	std::string police_id_;
	std::string police_password_;

public:
	// 水印部分内容
	std::string dev_name_;
	std::string frame_rate_;
	std::string bit_rate_;
	std::string trans_resolution_;

	std::string machine_id_;
	std::string bind_user_id_;		// 这个也和本地文件录像以及文件上传关联
	std::string bind_user_name_;	// 这个也和本地文件录像以及文件上传关联
	std::string dep_name_;
	std::string dep_id_;
	std::string show_location_;
	std::string show_datetime_;

public:
	// 这里用于存放需要回复给平台的请求数据
	// 还有一个配套的线程来
	std::queue<ExtraDataResponseInfo> extra_response_queue_;
	Poco::Event wait_queue_not_empty_;
	Poco::Thread extra_data_response_thread;
	static void  ExtraDataResponseThread(void *param);

public:
	GxxGmDSJSimulaterStreamMgr stream_mgr_;
	GxxGmDSJSimulaterNotifer *notifer_;

public:
	GxxGmWordSpeaker *speaker_;

public:
	Poco::Util::Application *app_;
};

#endif//_GxxGmDSJSimulater_H_
