#ifndef _GxxGmInputDevice_H_
#define _GxxGmInputDevice_H_

#include <windows.h>  
#include <map>  
#include <dshow.h>

#include "Poco/Thread.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavutil/avutil.h"
#include "libavutil/rational.h"
#include "libavutil/pixdesc.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

#include "GxxGmQueue.h"	

#define MAX_FRIENDLY_NAME_LENGTH    128  
#define MAX_MONIKER_NAME_LENGTH     256  

typedef struct _TDeviceName  
{  
	WCHAR FriendlyName[MAX_FRIENDLY_NAME_LENGTH];   // 设备友好名  
	WCHAR MonikerName[MAX_MONIKER_NAME_LENGTH];     // 设备Moniker名  
} TDeviceName;

class GxxGmInputDevice
{
public:
	GxxGmInputDevice();
	~GxxGmInputDevice();

public:
	/**
	 * 枚举本地接入的输入设备
	 */
	int EnumDevices();

private:
	int EnumInputDevices(std::map<int, TDeviceName> *devices, REFGUID guidValue);

public:
	std::map<int, TDeviceName> video_devices;
	std::map<int, TDeviceName> audio_devices;
};

//////////////////////////////////////////////////////////////////////////
//
//

class GxxGmDeviceObserver
{
public:
	virtual int MediaParam() = 0;
	virtual int MediaData(unsigned char *data, unsigned int len) = 0;
};

class GxxGmDevice
{
public:
	GxxGmDevice();
	~GxxGmDevice();

public:
	// 插入观察者，观察者能拿到视频参数和视频编码包
	int InsertObserver(GxxGmDeviceObserver *observer);
	// 移除观察者
	void RemoveObserver();

public:
	/**
	 * 打开设备
	 * 参数：
	 *	@device_name	设备名称
	 *	@device_type	设备类型：0-video，1-audio
	 */ 
	int Open(const char *device_name, int device_type = 0);

	// 开始拿数据包
	int Start();

	// 停止拿数据包
	void Stop();

	// 关闭设备
	void Close();

public:
	// 设备视音频数据读取线程
	Poco::Thread device_read_thread_;
	static void device_read_thread_fun(void *param);
	bool device_read_thread_exit_;

	// 编码转换线程
	Poco::Thread recode_thread_;
	static void recode_thread_fun(void *param);
	bool recode_thread_exit_;

	// 观察者分发模块
	Poco::Thread observer_dispatch_thread_;
	static void observer_dispatch_thread_fun(void *param);
	bool observer_dispatch_thread_exit_;

public:
	GxxGmDeviceObserver *observer_;

public:
	AVFormatContext *input_format_ctx_;
	AVInputFormat *iformat_;
	int device_type_;
	std::string device_name_;

	// 解码参数
	AVCodec *video_decoder_;
	AVCodec *audio_decoder_;
	AVCodecContext *video_decoder_ctx_;
	AVCodecContext *audio_decoder_ctx_;

	// 编码参数
	AVCodec *video_encoder_;
	AVCodec *audio_encoder_;
	AVCodecContext *video_encoder_ctx_;
	AVCodecContext *audio_encoder_ctx_;

	int input_video_stream_id_;
	int input_audio_stream_id_;

public:
	// 原始视频编码包队列
	GxxGmQueue<AVPacket> original_video_packet_queue_;
	GxxGmQueue<AVPacket> original_audio_packet_queue_;
	// 原始图像数据队列
	// 重编码后的编码包队列
	GxxGmQueue<AVPacket> recode_packet_queue_;
};

#endif//_GxxGmInputDevice_H_
