#include "GxxGmDSJSimulaterStreamMgr.h"
#include "GMFLib.h"
#include "PSFormat.h"

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
, stream_send_thread_handle_(NULL)
{
	// 
	av_register_all();
	avformat_network_init();
	avcodec_register_all();
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

	// ���ȼ���Token
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

	memset(current_token_, 0, 32);
	sprintf_s(current_token_, 32, "%d", streamHandle);

	ssrc_ = iSSRC;

	GSRTP_ERR err = GSRTPServer_SetSourceParam(current_token_, ssrc_, clientIP, clientPort, &struRate, 3*1024*1024);
	if (err != GSRTP_SUCCESS)
	{
		GSRTPServer_Reclaim(current_token_, ssrc_);
		return err;
	}

	// ���������߳�
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

int GxxGmDSJSimulaterStreamMgr::SendRealStream()
{
	// ��ָ����¼���ļ�����ȡ��Ƶ�������ͣ�������ѭ������
	// �������GMF�ļ�������

	// �ȴ�GMF�ļ�����ȡ��һ����������
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
		return errCode;
	}

	avformat_find_stream_info(input_format_context, NULL);

	int video_stream_index = -1;
	int audio_stream_index = -1;

	AVStream *video_stream = NULL;

	for (int index = 0; index < input_format_context->nb_streams; ++index)
	{
		if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = index;
			// ���ﳢ�Լ���һ��֡��
			video_stream = input_format_context->streams[index];
			Sleep(1);
		}
		else if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = index;
		}
	}

	AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");

	while (!is_stream_send_thread_stop_)
	{
		av_seek_frame(input_format_context)
	}

	while (!is_stream_send_thread_stop_)
	{
		AVPacket pkt;
		errCode = av_read_frame(input_format_context, &pkt);
		if (errCode < 0)
		{
			break;
		}

		if (pkt.stream_index == video_stream_index)
		{
			// ��Ƶ֡��ֱ�Ӵ����PS���
			// ���������Ҫȷ��һ�£�H.264�����Ƿ���Ҫ��һ�¹�����
			av_bitstream_filter_filter(h264bsfc, video_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);

			StruESStreamDesc es_stream_desc;
			es_stream_desc.eVideoCodecs = GS_MPEGPS_CODEC_V_H264;
			es_stream_desc.eAudioCodecs = GS_MPEGPS_CODEC_A_G711U;

			StruESFrameInfo es_frame_info;
			es_frame_info.eCodec = GS_MPEGPS_CODEC_V_H264;
			es_frame_info.eType = EnumGSMediaType::GS_MEDIA_TYPE_VIDEO;
			es_frame_info.nBufLen = pkt.size;
			es_frame_info.pBuffer = pkt.data;
			es_frame_info.nPTS = pkt.pts;

			while (true)
			{
				StruPSFrameInfo ps_frame;
				ps_frame.nBufLen = es_frame_info.nBufLen + 4096;
				ps_frame.pBuffer = new Byte[ps_frame.nBufLen];

				EnumGS_MPEGPS_RetCode err = GS_MPEGPS_ES2PS(&es_stream_desc, &es_frame_info, &ps_frame);
				if (err == GS_MPEGPS_Ret_Success)
				{
					// �ɹ��ˣ���PS������ȥ
					StruRtpFrame rtp_frame;
					rtp_frame.eFrameType = RTP_FRAME_PS;
					rtp_frame.iCodeID = 0x00000400;
					rtp_frame.iTimeStamp = ps_frame.nPTS;
					rtp_frame.iSSRC = ssrc_;
					rtp_frame.pFrame = (char*)ps_frame.pBuffer;
					rtp_frame.iLenght = ps_frame.nBufLen;

					GSRTP_ERR err = GSRTPServer_InputStream(current_token_, &rtp_frame);
					if (err != GSRTP_SUCCESS)
					{
						printf("����RTP��ʧ�ܣ������룺%d\n", err);
					}

					delete [] ps_frame.pBuffer;
					ps_frame.pBuffer = NULL;

					break;
				}
				else if (err == GS_MPEGPS_Ret_Out_Of_Buffer)
				{
					// ����������
					delete [] ps_frame.pBuffer;
					ps_frame.nBufLen += 4096;
					ps_frame.pBuffer = new Byte[ps_frame.nBufLen];
					continue;
				}
			}

			// �ֹ�����֡��Ϊ��25pfs
			Sleep(40);
		}
		else if (pkt.stream_index == audio_stream_index)
		{
			// ��Ƶ֡��ģ�����ݲ�������
		}
	}

	av_bitstream_filter_close(h264bsfc);
	avformat_close_input(&input_format_context);

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

	stream_mgr->SendRealStream();

	return ;
}
#endif

#ifdef WIN32_THREAD
DWORD WINAPI GxxGmDSJSimulaterStreamMgr::StreamSendThreadFuncEx(LPVOID* param)
{
	GxxGmDSJSimulaterStreamMgr *stream_mgr = (GxxGmDSJSimulaterStreamMgr*)param;

	stream_mgr->SendRealStream();

	return 0;
}
#endif