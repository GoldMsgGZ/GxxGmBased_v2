#include "stdafx.h"
#include "GB28181StreamMgr.h"
#include "GSStreamCodeID.h"


//////////////////////////////////////////////////////////////////////////
//
//

GB28181RealStream::GB28181RealStream()
{
	// 
}

GB28181RealStream::~GB28181RealStream()
{
	// 
}

int GB28181RealStream::MediaParam() 
{
	int errCode = 0;

	return errCode;
}

int GB28181RealStream::MediaData(unsigned char *data, unsigned int len)
{
	int errCode = 0;

	// 先尝试直接推送出去，如果不行的话，就打成PS流
	StruRtpFrame rtp_frame;
	rtp_frame.eFrameType = RTP_FRAME_I;
	rtp_frame.iCodeID = GS_CODEID_ST_H264;
	rtp_frame.pFrame = (char*)data;
	rtp_frame.iLenght = len;
	rtp_frame.iSSRC = SSRC_;
	rtp_frame.iTimeStamp = 0;
	GSRTP_ERR err = GSRTPServer_InputStream(token_, &rtp_frame);
	if (err != GSRTP_SUCCESS)
	{
		TRACE("GSRTPServer_InputStream %d\n", err);
	}

	return errCode;
}

int GB28181RealStream::InitStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort)
{
	int errCode = 0;
	memset(token_, 0, 32);
	sprintf_s(token_, 32, "%d", streamHandle);

	SSRC_ = iSSRC;

	errCode = GSRTPServer_AddSource(token_, SSRC_, &iLocalPort);

	return errCode;
}

int GB28181RealStream::StartStream(const char *device_id, STREAM_HANDLE streamHandle, int iSSRC, const char *clientIP, int clientPort)
{
	strncpy(deviceid_, device_id, 64);
	stream_handle_ = streamHandle;
	SSRC_ = iSSRC;
	strncpy(client_ip_, clientIP, 64);
	client_port_ = clientPort;

	StruRtpAVSampleRate rtp_av_samplerate;
	rtp_av_samplerate.iAudioSampleRate = 8000;
	rtp_av_samplerate.iVideoSampleRate = 90000;

	int errCode = rtp_stream_mgr_.SetSourceParam(token_, SSRC_, client_ip_, client_port_, &rtp_av_samplerate, 3*1024*1024);
	if (errCode < 0)
	{
		rtp_stream_mgr_.Reclain(token_, SSRC_);
		return -1;
	}

	is_stop_flag_ = false;
	stream_thread_.start(GB28181RealStream::StreamThread, this);
	Sleep(10);

	if (stream_thread_.isRunning())
		return 1;
	else
		return 0;
}

void GB28181RealStream::StopStream()
{
	is_stop_flag_ = false;
	stream_thread_.join();
}

void GB28181RealStream::StreamThread(void *param)
{
	// 准备好从指定的模块拿视频流，这里先用gmf文件顶上吧
	GB28181RealStream *real_stream = (GB28181RealStream *)param;

	//while (!real_stream->is_stop_flag_)
	//{
	//}
}

//////////////////////////////////////////////////////////////////////////
//
//

GB28181RealStreamMgr::GB28181RealStreamMgr()
{
	// 
}

GB28181RealStreamMgr::~GB28181RealStreamMgr()
{
	// 
}

int GB28181RealStreamMgr::InstallVideoDevice(const char *device_name)
{
	video_device_.Open(device_name);
	return 0;
}

int GB28181RealStreamMgr::InstallAudioDevice(const char *device_name)
{
	audio_device_.Open(device_name);
	return 0;
}

void GB28181RealStreamMgr::AddStream(const char *device_id, STREAM_HANDLE hStream, StruMediaInfo &remote_media_info, StruMediaInfo &local_media_info)
{
	// 创建一个流对象
	unsigned short local_port = 0;
	GB28181RealStream *stream = new GB28181RealStream();

	// 初始化流
	int errCode = stream->InitStream(hStream, remote_media_info.iSSRC, local_port);
	if (errCode < 0)
	{
		delete stream;
		return ;
	}

	local_media_info.iPort = local_port;

	// 向上级平台反馈结果
	GS28181_ERR iRet = GB28181Agent_RespondPlayRequest(hStream, eSTREAM_REALPLAY, &local_media_info, NULL);
	if (iRet != GS28181_ERR_SUCCESS)
	{
		delete stream;
		TRACE("[实时视频点播] 命令相应失败！错误码：%d\n", iRet);
		return ;
	}
	TRACE("[实时视频点播] 命令相应成功！错误码：%d\n", iRet);

	// 检查有多少个流，如果是0个，那么就启动设备，拉取视音频数据
	if (real_stream_map_.empty())
	{
		video_device_.Start();
		//audio_device_.Start();
	}

	video_device_.InsertObserver(stream);
	//audio_device_.InsertObserver(stream);

	// 启动流
	errCode = stream->StartStream(device_id, hStream, remote_media_info.iSSRC, remote_media_info.czIP, remote_media_info.iPort);
	if (errCode <= 0)
	{
		delete stream;
		TRACE("[实时视频点播] 启用流失败！错误码：%d\n", iRet);
		return ;
	}
	TRACE("[实时视频点播] 启用流成功！错误码：%d\n", iRet);

	// 流对象作为观察者，加入到设备观察者列表中

	// 将流信息加入映射表
	real_stream_map_.insert(std::pair<std::string, GB28181RealStream*>(device_id, stream));
}

void GB28181RealStreamMgr::RemoveStream(const char *device_id)
{
	//
	std::map<std::string, GB28181RealStream *>::iterator iter = real_stream_map_.find(device_id);

	if (iter == real_stream_map_.end())
		return ;

	iter->second->StopStream();
	delete iter->second;
	real_stream_map_.erase(iter);

	return ;
}

void GB28181RealStreamMgr::AddAudioStream(const char *device_id, STREAM_HANDLE hStream, StruMediaInfo &remote_media_info, StruMediaInfo &local_media_info)
{
	// 
}

//////////////////////////////////////////////////////////////////////////
//
//

GB28181HistoryStream::GB28181HistoryStream()
{
	// 
}

GB28181HistoryStream::~GB28181HistoryStream()
{
	// 
}

//////////////////////////////////////////////////////////////////////////
//
//

GB28181HistoryStreamMgr::GB28181HistoryStreamMgr()
{
	// 
}

GB28181HistoryStreamMgr::~GB28181HistoryStreamMgr()
{
	// 
}