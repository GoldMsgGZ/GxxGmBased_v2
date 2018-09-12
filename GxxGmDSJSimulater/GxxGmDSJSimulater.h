#ifndef _GxxGmDSJSimulater_H_
#define _GxxGmDSJSimulater_H_

#include "GxxGmDeviceInfo.h"
#include "GB28181AgentSDK.h"
#include <string>

class GxxGmDSJSimulater
{
public:
	GxxGmDSJSimulater();
	~GxxGmDSJSimulater();

public:
	// 开机
	int Initialize(const char *local_ip, const char *local_port, const char *local_gbcode, const char *server_ip, const char *server_port, const char *server_gbcode, const char *username, const char *password);

	// 关机
	void Destroy();

public:
	// 设置设备基本信息
	void SetBaseInfo(DEVICE_BASE_INFO base_info);
	// 设置设备定位信息
	void SetLocationInfo(DEVICE_LOCATION_INFO location_info);
	// 设置设备异常信息
	void SetExceptionInfo(DEVICE_EXCEPTION_INFO excep_info);

public:
	// 发送绑定用户请求
	int SendBindUserInfo(const char *platform_id, const char *device_imei, const char *user_id, const char *password);
	// 发送设备基本信息
	int SendBaseInfo();
	// 发送设备定位信息
	int SendLocationInfo();
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
	DEVICE_BASE_INFO base_info_;
	DEVICE_LOCATION_INFO location_info_;
	DEVICE_EXCEPTION_INFO exception_info_;

public:
	std::string bind_user_id_;
};

#endif//_GxxGmDSJSimulater_H_
