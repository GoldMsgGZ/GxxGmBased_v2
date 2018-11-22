#ifndef _FFMpegStub_H_
#define _FFMpegStub_H_

//#include <Windows.h>
#include "FFMpegInterface.h"

class FFMpegStub
{
public:
	FFMpegStub();
	~FFMpegStub();

public:
	int Initialize(const char *base_dir = NULL);
	void Destroy();

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// avformat.h

	_av_register_all			ptr_av_register_all;
	_avformat_network_init		ptr_avformat_network_init;
	_avformat_open_input		ptr_avformat_open_input;
	_avformat_close_input		ptr_avformat_close_input;
	_avformat_find_stream_info	ptr_avformat_find_stream_info;
	_av_seek_frame				ptr_av_seek_frame;
	_av_read_frame				ptr_av_read_frame;
	_av_free_packet				ptr_av_free_packet;
	_av_bitstream_filter_init	ptr_av_bitstream_filter_init;
	_av_bitstream_filter_filter	ptr_av_bitstream_filter_filter;
	_av_bitstream_filter_close	ptr_av_bitstream_filter_close;

public:
	_avcodec_register_all		ptr_avcodec_register_all;
	_av_copy_packet				ptr_av_copy_packet;
	_avcodec_find_decoder		ptr_avcodec_find_decoder;
	_avcodec_find_encoder		ptr_avcodec_find_encoder;
	_avcodec_open2				ptr_avcodec_open2;
	_avcodec_close				ptr_avcodec_close;
	_avcodec_decode_video2		ptr_avcodec_decode_video2;
	_avcodec_decode_audio4		ptr_avcodec_decode_audio4;
	_avcodec_encode_audio2		ptr_avcodec_encode_audio2;
	_avcodec_encode_video2		ptr_avcodec_encode_video2;

public:
	_av_free					ptr_av_free;
	_av_strerror				ptr_av_strerror;

private:
	void* h_avcodec_;
	void* h_avdevice_;
	void* h_avfilter_;
	void* h_avformat_;
	void* h_avutil_;
	void* h_postproc_;
	void* h_swresample_;
	void* h_swscale_;
};

#endif//_FFMpegStub_H_
