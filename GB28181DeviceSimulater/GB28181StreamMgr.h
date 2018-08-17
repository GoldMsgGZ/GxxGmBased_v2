#ifndef _GB28181StreamMgr_H_
#define _GB28181StreamMgr_H_

#include <map>

#include "Poco/Thread.h"

#include "GB28181AgentSDK.h"
#include "RtpStreamMgr.h"
#include "GxxGmInputDevice.h"



class GB28181RealStream : public GxxGmDeviceObserver
{
public:
	GB28181RealStream();
	~GB28181RealStream();

public:
	virtual int MediaParam();
	virtual int MediaData(unsigned char *data, unsigned int len);

public:
	int InitStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort);
	int StartStream(const char *device_id, STREAM_HANDLE streamHandle, int iSSRC, const char *clientIP, int clientPort);
	void StopStream();

public:
	static void StreamThread(void *param);

private:
	Poco::Thread stream_thread_;

public:
	char token_[32];
	int SSRC_;
	char deviceid_[64];
	STREAM_HANDLE stream_handle_;
	char client_ip_[64];
	int client_port_;
	bool is_stop_flag_;
};

class GB28181RealStreamMgr
{
public:
	GB28181RealStreamMgr();
	~GB28181RealStreamMgr();

public:
	int InstallVideoDevice(const char *device_name);
	int InstallAudioDevice(const char *device_name);

public:
	/**
	 * 增加一路流
	 * 参数：
	 *	@device_id				设备/通道国标码
	 *	@hStream				流句柄
	 *	@remote_media_info		远端/上级媒体信息
	 *	@local_media_info		本地媒体信息
	 */
	void AddStream(const char *device_id, STREAM_HANDLE hStream,  StruMediaInfo &remote_media_info,  StruMediaInfo &local_media_info);
	void RemoveStream(const char *device_id);
	void AddAudioStream(const char *device_id, STREAM_HANDLE hStream,  StruMediaInfo &remote_media_info,  StruMediaInfo &local_media_info);

private:
	std::map<std::string, GB28181RealStream*> real_stream_map_;

private:
	GxxGmDevice video_device_;
	GxxGmDevice audio_device_;
};

//////////////////////////////////////////////////////////////////////////

class GB28181HistoryStream
{
public:
	GB28181HistoryStream();
	~GB28181HistoryStream();

public:
};

class GB28181HistoryStreamMgr
{
public:
	GB28181HistoryStreamMgr();
	~GB28181HistoryStreamMgr();

public:

private:
	std::map<std::string, GB28181HistoryStream*> history_stream_map_;
};

#endif//_GB28181StreamMgr_H_
