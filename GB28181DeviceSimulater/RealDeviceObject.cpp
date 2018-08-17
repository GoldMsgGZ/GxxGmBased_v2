#include "stdafx.h"
#include "RealDeviceObject.h"

RealDeviceObject::RealDeviceObject(RealDeviceObjectObserver *observer)
: input_format_ctx_(NULL)
, iformat_(NULL)
, video_decoder_(NULL)
, audio_decoder_(NULL)
, video_decoder_ctx_(NULL)
, audio_decoder_ctx_(NULL)
, dest_codec_id_(AV_CODEC_ID_NONE)
, video_encoder_(NULL)
, audio_encoder_(NULL)
, video_encoder_ctx_(NULL)
, audio_encoder_ctx_(NULL)
, input_video_stream_id_(-1)
, input_audio_stream_id_(-1)
, observer_(observer)
{
	av_register_all();
	avcodec_register_all();
	avformat_network_init();
}

RealDeviceObject::~RealDeviceObject()
{
	avformat_network_deinit();
}

int RealDeviceObject::SetDeviceName(const char *device_name, enum MediaDeviceType dev_type /* = Dev_Camera */, const char *open_type /* = "dshow" */)
{
	int errCode = 0;

	device_name_ = device_name;
	device_type_ = dev_type;
	open_type_ = open_type;

	return errCode;
}

int RealDeviceObject::OpenRealDevice()
{
	int errCode = 0;

	input_format_ctx_ = avformat_alloc_context();
	iformat_ = av_find_input_format(open_type_.c_str());

	char device_symbolink[128] = {0};
	if (device_type_ == Dev_Camera)
		sprintf_s(device_symbolink, 128, "video=%s", device_name_.c_str());
	else if (device_type_ == Dev_Mic)
		sprintf_s(device_symbolink, 128, "mic=%s", device_name_.c_str());

	device_symbolink_ = device_symbolink;

	return errCode;
}

void RealDeviceObject::CloseRealDevice()
{
	//return 0;
}

int RealDeviceObject::Start(enum AVCodecID dest_codec_id)
{
	int errCode = 0;

	if (stream_handler_.isRunning())
		return -1;

	dest_codec_id_ = dest_codec_id;

	// 启动工作线程
	is_stream_handler_stop_ = false;
	stream_handler_.start(RealDeviceObject::StreamHandler, this);

	return errCode;
}

int RealDeviceObject::Stop()
{
	is_stream_handler_stop_ = true;
	stream_handler_.join();

	return 0;
}

void RealDeviceObject::StreamHandler(void *param)
{
	RealDeviceObject *object = (RealDeviceObject*)param;
	char dbg_msg[4096] = {0};

	int errCode = avformat_open_input(&object->input_format_ctx_, object->device_symbolink_.c_str(), object->iformat_, NULL);
	if (errCode < 0)
	{
		char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
		av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
		sprintf_s(dbg_msg, 4096, "打开物理设备出错，错误信息：%s\n", av_msg);
		OutputDebugStringA(dbg_msg);
		return ;
	}

	avformat_find_stream_info(object->input_format_ctx_, NULL);

	for (int index = 0; index < object->input_format_ctx_->nb_streams; ++index)
	{
		if (object->input_format_ctx_->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			object->input_video_stream_id_ = index;

			// 找解码器
			object->video_decoder_ = avcodec_find_decoder(object->input_format_ctx_->streams[index]->codec->codec_id);
			if (object->video_decoder_ == NULL)
			{
				// 没找到，要通知上层停止
				return;
			}

			// 好像调取设备流不用单独申请上下文了
			object->video_decoder_ctx_ = object->input_format_ctx_->streams[index]->codec;

			errCode = avcodec_open2(object->video_decoder_ctx_, object->video_decoder_, NULL);
			if (errCode < 0)
			{
				// 打开解码器失败
				char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
				av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				sprintf_s(dbg_msg, 4096, "打开视频解码器出错，错误信息：%s\n", av_msg);
				OutputDebugStringA(dbg_msg);
				return ;
			}
		}
		else if (object->input_format_ctx_->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			object->input_audio_stream_id_ = index;

			// 找解码器
			object->audio_decoder_ = avcodec_find_decoder(object->input_format_ctx_->streams[index]->codec->codec_id);
			if (object->audio_decoder_ == NULL)
			{
				// 没找到，要通知上层停止
				return;
			}

			// 好像调取设备流不用单独申请上下文了
			object->audio_decoder_ctx_ = object->input_format_ctx_->streams[index]->codec;

			errCode = avcodec_open2(object->audio_decoder_ctx_, object->audio_decoder_, NULL);
			if (errCode < 0)
			{
				// 打开解码器失败
				char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
				av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				sprintf_s(dbg_msg, 4096, "打开音频解码器出错，错误信息：%s\n", av_msg);
				OutputDebugStringA(dbg_msg);
				return ;
			}
		}
	}

	// 准备编码环境
	if (object->device_type_ == Dev_Camera)
	{
		object->video_encoder_ = avcodec_find_encoder(object->dest_codec_id_);
		if (object->video_encoder_ == NULL)
		{
			// 没找到编码器
			return ;
		}

		object->video_encoder_ctx_ = avcodec_alloc_context3(object->video_encoder_);
		if (object->video_encoder_ctx_ == NULL)
		{
			// 申请编码器上下文内存失败！
			return ;
		}
	}
	else if (object->device_type_ == Dev_Mic)
	{
		object->audio_decoder_ = avcodec_find_encoder(object->dest_codec_id_);
		if (object->audio_decoder_ == NULL)
		{
			// 没找到编码器
			return ;
		}

		object->audio_encoder_ctx_ = avcodec_alloc_context3(object->audio_decoder_);
		if (object->audio_encoder_ctx_ == NULL)
		{
			// 申请编码器上下文内存失败！
			return ;
		}
	}
	

	// 组织编码参数
	object->video_encoder_ctx_->width = object->video_decoder_ctx_->width;
	object->video_encoder_ctx_->height = object->video_decoder_ctx_->height;
	object->video_encoder_ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
	object->video_encoder_ctx_->time_base.num = 1;
	object->video_encoder_ctx_->time_base.den = 30;
	object->video_encoder_ctx_->bit_rate = 600000;

	av_opt_set(object->video_encoder_ctx_->priv_data, "preset", "slow", 0);
	av_opt_set(object->video_encoder_ctx_->priv_data, "tune", "zerolatency", 0);

	errCode = avcodec_open2(object->video_encoder_ctx_, object->video_encoder_, NULL);
	if (errCode < 0)
	{
		// 打开编码器失败！
		char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
		av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
		sprintf_s(dbg_msg, 4096, "打开视频编码器出错，错误信息：%s\n", av_msg);
		OutputDebugStringA(dbg_msg);
		return ;
	}

	// 开始转码发送
	while (!object->is_stream_handler_stop_)
	{
		AVPacket encoded_packet;
		errCode = av_read_frame(object->input_format_ctx_, &encoded_packet);
		if (errCode < 0)
		{
			char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
			av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
			sprintf_s(dbg_msg, 4096, "读取媒体编码帧出错，错误信息：%s\n", av_msg);
			OutputDebugStringA(dbg_msg);
			break;
		}

		AVFrame *frame = av_frame_alloc();
		int got_frame = 0;

		if (encoded_packet.stream_index == object->input_video_stream_id_)
		{
			errCode = avcodec_decode_video2(object->video_decoder_ctx_, frame, &got_frame, &encoded_packet);
			if (errCode < 0)
			{
				// 解码失败
				char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
				av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				sprintf_s(dbg_msg, 4096, "视频解码出错，错误信息：%s\n", av_msg);
				OutputDebugStringA(dbg_msg);

				av_frame_free(&frame);
				av_free_packet(&encoded_packet);
				continue;
			}

			if (!got_frame)
			{
				av_frame_free(&frame);
				av_free_packet(&encoded_packet);
				continue;
			}

			// 输出部分调试信息
			if (frame->pict_type == AV_PICTURE_TYPE_I)
				sprintf_s(dbg_msg, 4096, "视频解码 得到 I 帧\n");
			else if (frame->pict_type == AV_PICTURE_TYPE_B)
				sprintf_s(dbg_msg, 4096, "视频解码 得到 B 帧\n");
			else if (frame->pict_type == AV_PICTURE_TYPE_P)
				sprintf_s(dbg_msg, 4096, "视频解码 得到 P 帧\n");
			else
				sprintf_s(dbg_msg, 4096, "视频解码 其他类型视频帧\n");

			OutputDebugStringA(dbg_msg);

			// 重新编码
			AVPacket recoded_packet;
			recoded_packet.buf = NULL;
			recoded_packet.size = 0;
			av_init_packet(&recoded_packet);
			int got_packet = 0;
			errCode = avcodec_encode_video2(object->video_encoder_ctx_, &recoded_packet, frame, &got_packet);
			if (errCode < 0)
			{
				// 编码失败
				char av_msg[AV_ERROR_MAX_STRING_SIZE] = {0};
				av_make_error_string(av_msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				sprintf_s(dbg_msg, 4096, "视频编码出错，错误信息：%s\n", av_msg);
				OutputDebugStringA(dbg_msg);

				av_frame_free(&frame);
				av_free_packet(&encoded_packet);
				av_free_packet(&recoded_packet);

				continue;
			}

			if (!got_packet)
			{
				av_frame_free(&frame);
				av_free_packet(&encoded_packet);
				av_free_packet(&recoded_packet);
				continue;
			}

			sprintf_s(dbg_msg, 4096, "视频编码成功!\n");
			OutputDebugStringA(dbg_msg);

			// 做PS打包，然后发出去
			StruESStreamDesc desc;
			desc.eVideoCodecs = GS_MPEGPS_CODEC_V_H264;
			desc.eAudioCodecs = GS_MPEGPS_CODEC_A_G711U;

			StruESFrameInfo es_frame;
			es_frame.eCodec = GS_MPEGPS_CODEC_V_H264;
			es_frame.eType = GS_MEDIA_TYPE_VIDEO;
			es_frame.nPTS = encoded_packet.pts;
			es_frame.pBuffer = recoded_packet.data;
			es_frame.nBufLen = recoded_packet.size;

			StruPSFrameInfo ps_frame;
			ps_frame.nBufLen = 4096;
			ps_frame.pBuffer = new Byte[ps_frame.nBufLen];

			while (true)
			{
				EnumGS_MPEGPS_RetCode err = GS_MPEGPS_ES2PS(&desc, &es_frame, &ps_frame);
				if (err == GS_MPEGPS_Ret_Success)
				{
					// 操作成功了，发送出去
					errCode = object->observer_->MediaData(&ps_frame);
					if (errCode != 0)
					{
						sprintf_s(dbg_msg, 4096, "发送视频流出错，错误码：%d\n", errCode);
						OutputDebugStringA(dbg_msg);
					}

					delete [] ps_frame.pBuffer;
					break;
				}
				else if (err == GS_MPEGPS_Ret_Out_Of_Buffer)
				{
					// 缓冲区不够
					delete [] ps_frame.pBuffer;
					ps_frame.nBufLen += 1024;
					ps_frame.pBuffer = new Byte[ps_frame.nBufLen];
					continue;
				}
				else
				{
					// PS封装出错
					sprintf_s(dbg_msg, 4096, "视频流打包PS出错，错误码：%d\n", err);
					OutputDebugStringA(dbg_msg);
					delete [] ps_frame.pBuffer;
					break;
				}
			}

			av_frame_free(&frame);
			av_free_packet(&encoded_packet);
			av_free_packet(&recoded_packet);
		}
		else if (encoded_packet.stream_index == object->input_audio_stream_id_)
		{
		}
		
	}
}