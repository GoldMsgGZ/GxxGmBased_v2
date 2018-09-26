#include "GxxGmDSJSimulaterStreamMgr.h"
#include "GMFLib.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#ifdef __cplusplus
};
#endif

GxxGmDSJSimulaterStreamMgr::GxxGmDSJSimulaterStreamMgr()
: is_stream_send_thread_stop_(false)
{
	// 
}

GxxGmDSJSimulaterStreamMgr::~GxxGmDSJSimulaterStreamMgr()
{
	// 
}

int GxxGmDSJSimulaterStreamMgr::Initialize(int is_manual_port, unsigned short begin_port, unsigned short end_port, const char *local_ip)
{
	StruRtpPort rtp_port;
	rtp_port.eMode = is_manual_port == 1 ? RTP_PORT_MANUAL : RTP_PORT_AUTO;
	rtp_port.iBeginPort = begin_port;
	rtp_port.iEndPort = end_port;

	GSRTP_ERR err = GSRTPServer_Init(&rtp_port, local_ip, RTP_OVER_UDP, GxxGmDSJSimulaterStreamMgr::_RtpServerEventCallBack, this, 5);
	return err;
}

void GxxGmDSJSimulaterStreamMgr::UnInitialize()
{
	GSRTPServer_Cleanup();
}

int GxxGmDSJSimulaterStreamMgr::AddRealStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort)
{
	int errCode = 0;

	// 首先计算Token
	char current_token[32] = {0};
	memset(current_token, 0, 32);
	sprintf_s(current_token, 32, "%d", streamHandle);

	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string tmp = current_program_path;
	int pos = tmp.find_last_of('\\');

	std::string video_path = tmp.substr(0, pos + 1);
	video_path.append("video.gmf");

	errCode = GSRTPServer_AddSource(current_token, iSSRC, &iLocalPort);
	if (errCode == GSRTP_SUCCESS)
	{
		RtpStreamInfo *rtp_info = new RtpStreamInfo;
		memset(rtp_info->stream_token_, 0, 32);
		sprintf_s(rtp_info->stream_token_, 32, "%d", streamHandle);

		rtp_info->SSRC_ = iSSRC;
		rtp_info->stream_type_ = RtpStream_Real;

		int count = stream_maps_.size();
		stream_maps_.insert(std::pair<int, RtpStreamInfo *>(count, rtp_info));
	}

	return errCode;
}

int GxxGmDSJSimulaterStreamMgr::StartRealStream(STREAM_HANDLE streamHandle, int iSSRC, const char *clientIP, int clientPort)
{
	StruRtpAVSampleRate struRate;
	struRate.iAudioSampleRate = 8000;
	struRate.iVideoSampleRate = 90000;

	char current_token[32] = {0};
	memset(current_token, 0, 32);
	sprintf_s(current_token, 32, "%d", streamHandle);

	GSRTP_ERR err = GSRTPServer_SetSourceParam(current_token, iSSRC, clientIP, clientPort, &struRate, 3*1024*1024);
	if (err != GSRTP_SUCCESS)
	{
		GSRTPServer_Reclaim(current_token, iSSRC);
		return err;
	}

	// 启动推流线程
	if (!stream_send_thread_.isRunning())
	{
		stream_send_thread_.start(GxxGmDSJSimulaterStreamMgr::StreamSendThreadFunc, this);
	}

	return 0;
}

void GS_RTP_CALLBACK GxxGmDSJSimulaterStreamMgr::_RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData)
{
	// 
}

void GxxGmDSJSimulaterStreamMgr::StreamSendThreadFunc(void* param)
{
	GxxGmDSJSimulaterStreamMgr *stream_mgr = (GxxGmDSJSimulaterStreamMgr*)param;
	// 从指定的录像文件中拉取视频流并推送，并且是循环推送
	// 这里采用GMF文件来推流

	// 先打开GMF文件，读取到一个队列里面
	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string tmp = current_program_path;
	int pos = tmp.find_last_of('\\');

	std::string video_path = tmp.substr(0, pos + 1);
	video_path.append("video.gmf");

	AVFormatContext *input_format_context = NULL;
	int errCode = avformat_open_input(&input_format_context, video_path.c_str(), NULL, NULL);
	if (errCode < 0)
	{
		return ;
	}

	avformat_find_stream_info(input_format_context, NULL);

	int video_stream_index = -1;
	int audio_stream_index = -1;

	for (int index = 0; index < input_format_context->nb_streams; ++index)
	{
		if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = index;
		}
	}
}