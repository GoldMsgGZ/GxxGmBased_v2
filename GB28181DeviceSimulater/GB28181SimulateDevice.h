#ifndef _GB28181SimulateDevice_H_
#define _GB28181SimulateDevice_H_

#include "GB28181Defs.h"
#include "RealDeviceObject.h"
#include "RtpServerObject.h"
#include <string>

/**
 * 28181模拟设备，应当包含以下几个实体：
 * - 摄像头
 * - 拾音器
 * - 录像机
 */
class GB28181SimulateDevice : public RealDeviceObjectObserver
{
public:
	GB28181SimulateDevice();
	~GB28181SimulateDevice();

public:
	int SetDeviceGbcode(const char *device_gbcode);
	int SetVideoGbcode(const char *video_gbcode);
	int SetVideoDeviceName(const char *device_name);

public:
	int GetDeviceCatlog(int &catlog_counts, StruCatalogInfo **catlogs);
	int GetDeviceInfo(StruDeviceInfo *device_info);
	int GetDeviceStatus(StruDeviceStatus *device_status);
	int GetDeviceRecordInfo(const char *start_time, const char *end_time, StruRecordIndex *record_info);

public:
	int AddRealStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort);
	int StartRealStream(STREAM_HANDLE streamHandle, StruMediaInfo *remote_media_info);

public:
	int StopStream(STREAM_HANDLE streamHandle);

public:
	virtual int MediaData(void *stream_handle, int ssrc, StruPSFrameInfo *ps_frame);

private:
	std::string device_gbcode_;
	std::string video_gbcode_;

public:
	RealDeviceObject *camera_;
	RealDeviceObject *mic_;
	RtpStreamMgr *rtp_stream_mgr_;

public:
	//char token_[32];
	//int SSRC_;
};

#endif
