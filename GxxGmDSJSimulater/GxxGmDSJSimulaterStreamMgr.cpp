#include "GxxGmDSJSimulaterStreamMgr.h"
#include "PSFormat.h"
#include "GB28181AgentSDK.h"


GxxGmDSJSimulaterStreamMgr::GxxGmDSJSimulaterStreamMgr()
: is_stream_send_thread_stop_(false)
, stream_send_thread_handle_(NULL)
{
	//
}

GxxGmDSJSimulaterStreamMgr::~GxxGmDSJSimulaterStreamMgr()
{
	// 
}

int GxxGmDSJSimulaterStreamMgr::Initialize(int is_manual_port, unsigned short begin_port, unsigned short end_port, const char *local_ip, const char *rtp_net, const char *stream_file)
{
	//char current_program_path[4096] = {0};
	//GetModuleFileNameA(NULL, current_program_path, 4096);
	//std::string tmp = current_program_path;
	//int pos = tmp.find_last_of('\\');

	//// 这里初始化FFMpeg
	//std::string ffmpeg_base_dir = tmp.substr(0, pos + 1);
	//ffmpeg_.Initialize(ffmpeg_base_dir.c_str());

	rtp_net_ = rtp_net;
	stream_file_ = stream_file;

	EnumRtpNetType rtp_net_type = RTP_OVER_UDP;
	if (rtp_net_.compare("UDP") == 0)
	{
		rtp_net_type = EnumRtpNetType::RTP_OVER_UDP;
	}
	else if (rtp_net_.compare("TCP") == 0)
	{
		rtp_net_type = EnumRtpNetType::RTP_OVER_TCP_ACTIVE;
	}

	StruRtpPort rtp_port;
	rtp_port.eMode = is_manual_port == 1 ? RTP_PORT_MANUAL : RTP_PORT_AUTO;
	rtp_port.iBeginPort = begin_port;
	rtp_port.iEndPort = end_port;

	GSRTP_ERR err = GSRTPServer_Init(&rtp_port, local_ip, rtp_net_type, GxxGmDSJSimulaterStreamMgr::_RtpServerEventCallBack, this, 5);
	return err;
}

void GxxGmDSJSimulaterStreamMgr::UnInitialize()
{
	GSRTPServer_Cleanup();
}

void GxxGmDSJSimulaterStreamMgr::SetFFmpegStub(FFMpegStub *ffmpeg)
{
	ffmpeg_ = ffmpeg;
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
	return errCode;
}

int GxxGmDSJSimulaterStreamMgr::StartRealStream(STREAM_HANDLE streamHandle, int iSSRC, const char *clientIP, int clientPort)
{
	StruRtpAVSampleRate struRate;
	struRate.iAudioSampleRate = 8000;
	struRate.iVideoSampleRate = 90000;

	memset(current_token_, 0, 32);
	sprintf_s(current_token_, 32, "%d", streamHandle);

	ssrc_ = iSSRC;
	streamHandle_ = streamHandle;

	GSRTP_ERR err = GSRTPServer_SetSourceParam(current_token_, ssrc_, clientIP, clientPort, &struRate, 3*1024*1024);
	if (err != GSRTP_SUCCESS)
	{
		GSRTPServer_Reclaim(current_token_, ssrc_);
		return err;
	}

	// 启动推流线程
#ifdef POCO_THREAD
	if (!stream_send_thread_.isRunning())
	{
		stream_send_thread_.start(GxxGmDSJSimulaterStreamMgr::StreamSendThreadFunc, this);
	}
#endif

#ifdef WIN32_THREAD
	DWORD exit_code = 0;
	BOOL bret = GetExitCodeThread(stream_send_thread_handle_, &exit_code);
	if (exit_code != STILL_ACTIVE)
	{
		stream_send_thread_handle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GxxGmDSJSimulaterStreamMgr::StreamSendThreadFuncEx, this, 0, NULL);
	}
#endif

	return 0;
}

int GxxGmDSJSimulaterStreamMgr::StopRealStream()
{
	is_stream_send_thread_stop_ = true;

#ifdef POCO_THREAD
	stream_send_thread_.join();
#endif

#ifdef WIN32_THREAD
	while (true)
	{
		DWORD exit_code = 0;
		BOOL bret = GetExitCodeThread(stream_send_thread_handle_, &exit_code);
		if (exit_code != STILL_ACTIVE)
			break;

		Sleep(1000);
	}
#endif

	return 0;
}

int GxxGmDSJSimulaterStreamMgr::SendRealStream()
{
	// 从指定的录像文件中拉取视频流并推送，并且是循环推送
	// 这里采用GMF文件来推流
	char ffmppeg_errpr[4096] = {0};

	// 先打开GMF文件，读取到一个队列里面
	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string tmp = current_program_path;
	int pos = tmp.find_last_of('\\');

	std::string video_path = tmp.substr(0, pos + 1);
	video_path.append(stream_file_.c_str());

	AVFormatContext *input_format_context = NULL;
	int errCode = ffmpeg_->ptr_avformat_open_input(&input_format_context, video_path.c_str(), NULL, NULL);
	if (errCode < 0)
	{
		ffmpeg_->ptr_av_strerror(errCode, ffmppeg_errpr, 4096);
		return errCode;
	}

	errCode = ffmpeg_->ptr_avformat_find_stream_info(input_format_context, NULL);
	if (errCode < 0)
	{
		ffmpeg_->ptr_av_strerror(errCode, ffmppeg_errpr, 4096);
		return errCode;
	}

	int video_stream_index = -1;
	int audio_stream_index = -1;

	AVStream *video_stream = NULL;
	AVCodecID video_codec_id = AV_CODEC_ID_NONE;
	AVCodecID audio_codec_id = AV_CODEC_ID_NONE;
	EnumGSPSCodecType gxx_codec_id = EnumGSPSCodecType::GS_MPEGPS_CODEC_NONE;
	int video_frame_rate = 0;

	for (int index = 0; index < input_format_context->nb_streams; ++index)
	{
		if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = index;
			// 这里尝试计算一下帧率
			video_stream = input_format_context->streams[index];

			// 如果帧率参数不对，那么我们就强制设置为30帧
			if (video_stream->avg_frame_rate.den == 0)
				video_frame_rate = 30;
			else
				video_frame_rate = video_stream->avg_frame_rate.num / video_stream->avg_frame_rate.den;

			// 这里拿一下编码，看看后面能不能兼容其他的编码格式
			video_codec_id = video_stream->codec->codec_id;
			if (video_codec_id == AV_CODEC_ID_H264)
			{
				gxx_codec_id = EnumGSPSCodecType::GS_MPEGPS_CODEC_V_H264;
				printf("视频编码为：AVC(H264)\n");
			}
			else if (video_codec_id == AV_CODEC_ID_H265)
			{
				gxx_codec_id = EnumGSPSCodecType::GS_MPEGPS_CODEC_V_H265;
				printf("视频编码为：HEVC(H265)\n");
			}
			else if (video_codec_id == AV_CODEC_ID_MPEG4)
			{
				gxx_codec_id = EnumGSPSCodecType::GS_MPEGPS_CODEC_V_MP4;
				printf("视频编码为：MP4\n");
			}
			else if (video_codec_id == AV_CODEC_ID_MPEG1VIDEO)
			{
				gxx_codec_id = EnumGSPSCodecType::GS_MPEGPS_CODEC_V_MPEG1;
				printf("视频编码为：MPEG1\n");
			}
			else if (video_codec_id == AV_CODEC_ID_MPEG2VIDEO)
			{
				gxx_codec_id = EnumGSPSCodecType::GS_MPEGPS_CODEC_V_MPEG2;
				printf("视频编码为：MPEG2\n");
			}
			else
			{
				printf("不支持的视频编码...请转码...\n");
				system("pause");
			}
		}
		else if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = index;
			audio_codec_id = input_format_context->streams[index]->codec->codec_id;
		}
	}

	// 计算帧率间隔
	int wait_time = 1000 / video_frame_rate;

	AVBitStreamFilterContext* h264bsfc = ffmpeg_->ptr_av_bitstream_filter_init("h264_mp4toannexb");

	while (!is_stream_send_thread_stop_)
	{
		// 回到视频开始处，对于有些视频似乎不太起作用
		ffmpeg_->ptr_av_seek_frame(input_format_context, video_stream_index, 0, AVSEEK_FLAG_BACKWARD);

		while (!is_stream_send_thread_stop_)
		{
			AVPacket pkt;
			errCode = ffmpeg_->ptr_av_read_frame(input_format_context, &pkt);
			if (errCode < 0)
			{
				// 这里要识别错误
				ffmpeg_->ptr_av_strerror(errCode, ffmppeg_errpr, 4096);
				break;
			}

			if (pkt.size == 0)
			{
				ffmpeg_->ptr_av_free_packet(&pkt);
				continue;
			}

			if (pkt.stream_index == video_stream_index)
			{
				// 视频帧，直接打包成PS封包
				// 这里可能需要确认一下，H.264编码是否需要加一下过滤器
				// 这么操作会导致内存泄露，需要解决以下。参考页面：https://blog.csdn.net/LG1259156776/article/details/73283920
				AVPacket out_pkt;
				if (gxx_codec_id == EnumGSPSCodecType::GS_MPEGPS_CODEC_V_H264)
					ffmpeg_->ptr_av_bitstream_filter_filter(h264bsfc, video_stream->codec, NULL, &out_pkt.data, &out_pkt.size, pkt.data, pkt.size, 0);
				else
					ffmpeg_->ptr_av_copy_packet(&out_pkt, &pkt);

				StruESStreamDesc es_stream_desc;
				es_stream_desc.eVideoCodecs = gxx_codec_id;
				es_stream_desc.eAudioCodecs = GS_MPEGPS_CODEC_A_G711;

				StruESFrameInfo es_frame_info;
				es_frame_info.eCodec = gxx_codec_id;
				es_frame_info.eType = EnumGSMediaType::GS_MEDIA_TYPE_VIDEO;
				es_frame_info.nBufLen = out_pkt.size;
				es_frame_info.pBuffer = out_pkt.data;
				es_frame_info.nPTS = out_pkt.pts;

				int len = es_frame_info.nBufLen + 4096;
				while (true)
				{
					StruPSFrameInfo *ps_frame = new StruPSFrameInfo;
					ps_frame->nBufLen = len;
					ps_frame->pBuffer = new Byte[ps_frame->nBufLen];

					EnumGS_MPEGPS_RetCode err = GS_MPEGPS_ES2PS(&es_stream_desc, &es_frame_info, ps_frame);
					if (err == GS_MPEGPS_Ret_Success)
					{
						// 成功了，将PS包发出去
						StruRtpFrame rtp_frame;
						rtp_frame.eFrameType = RTP_FRAME_PS;
						rtp_frame.iCodeID = 0x00000400;
						rtp_frame.iTimeStamp = ps_frame->nPTS;
						rtp_frame.iSSRC = ssrc_;
						rtp_frame.pFrame = (char*)ps_frame->pBuffer;
						rtp_frame.iLenght = ps_frame->nBufLen;

						GSRTP_ERR err = GSRTPServer_InputStream(current_token_, &rtp_frame);
						if (err != GSRTP_SUCCESS)
						{
							printf("发送视频RTP包失败！错误码：%d\n", err);
						}

						delete [] ps_frame->pBuffer;
						ps_frame->pBuffer = NULL;

						delete ps_frame;
						ps_frame = NULL;

						break;
					}
					else if (err == GS_MPEGPS_Ret_Out_Of_Buffer)
					{
						// 缓冲区不够
						delete [] ps_frame->pBuffer;
						ps_frame->pBuffer = NULL;

						delete ps_frame;
						ps_frame = NULL;

						len += 4096;
						continue;
					}

					delete [] ps_frame->pBuffer;
					ps_frame->pBuffer = NULL;

					delete ps_frame;
					ps_frame = NULL;
				}

				if (gxx_codec_id == EnumGSPSCodecType::GS_MPEGPS_CODEC_V_H264)
					ffmpeg_->ptr_av_free(out_pkt.data);
				else
					ffmpeg_->ptr_av_free_packet(&out_pkt);

				// 控制帧率
				Sleep(wait_time);
			}
			else if (pkt.stream_index == audio_stream_index)
			{
				// 音频帧，模拟器暂不处理吧
				if ((audio_codec_id == AV_CODEC_ID_MP2) || (audio_codec_id == AV_CODEC_ID_PCM_ALAW) || (audio_codec_id == AV_CODEC_ID_PCM_MULAW))
				{
					// audio_codec_id为192的时候这里我们单方面认为是G711
					// 直接拿出音频数据扔到包内
					StruRtpFrame rtp_frame;
					rtp_frame.eFrameType = RTP_FRAME_PS;
					rtp_frame.iCodeID = 0x00000400;
					rtp_frame.iTimeStamp = pkt.pts;
					rtp_frame.iSSRC = ssrc_;
					rtp_frame.pFrame = (char*)pkt.data;
					rtp_frame.iLenght = pkt.size;

					GSRTP_ERR err = GSRTPServer_InputStream(current_token_, &rtp_frame);
					if (err != GSRTP_SUCCESS)
					{
						printf("发送音频RTP包失败！错误码：%d\n", err);
					}
				}
			}

			ffmpeg_->ptr_av_free_packet(&pkt);
		}
	}

	ffmpeg_->ptr_av_bitstream_filter_close(h264bsfc);
	ffmpeg_->ptr_avformat_close_input(&input_format_context);

	return 0;
}

void GS_RTP_CALLBACK GxxGmDSJSimulaterStreamMgr::_RtpServerEventCallBack(const char *szToken, unsigned int iSSRC, EnumRtpEventType eEvent, void *pEventData, void *pUserData)
{
	// 
}

#ifdef POCO_THREAD
void GxxGmDSJSimulaterStreamMgr::StreamSendThreadFunc(void* param)
{
	GxxGmDSJSimulaterStreamMgr *stream_mgr = (GxxGmDSJSimulaterStreamMgr*)param;
	stream_mgr->is_stream_send_thread_stop_ = false;
	stream_mgr->SendRealStream();

	return ;
}
#endif

#ifdef WIN32_THREAD
DWORD WINAPI GxxGmDSJSimulaterStreamMgr::StreamSendThreadFuncEx(LPVOID* param)
{
	GxxGmDSJSimulaterStreamMgr *stream_mgr = (GxxGmDSJSimulaterStreamMgr*)param;
	stream_mgr->is_stream_send_thread_stop_ = false;
	stream_mgr->SendRealStream();

	return 0;
}
#endif
