#include "FFMpegStub.h"
#include <string>
#include <Windows.h>

#define LIB_AVFORMAT	"avformat-57.dll"
#define LIB_AVCODEC		"avcodec-57.dll"
#define LIB_AVUTIL		"avutil-55.dll"
#define LIB_AVFILTER	"avfilter-6.dll"
#define LIB_AVDEVICE	"avdevice-57.dll"
#define LIB_POSTPROC	"postproc-54.dll"
#define LIB_SWRESAMPLE	"swresample-2.dll"
#define LIB_SWSCALE		"swscale-4.dll"

FFMpegStub::FFMpegStub()
: h_avcodec_(NULL)
, h_avdevice_(NULL)
, h_avfilter_(NULL)
, h_avformat_(NULL)
, h_avutil_(NULL)
, h_postproc_(NULL)
, h_swresample_(NULL)
, h_swscale_(NULL)
, ptr_av_register_all(NULL)
, ptr_avformat_network_init(NULL)
, ptr_avformat_open_input(NULL)
, ptr_avformat_close_input(NULL)
, ptr_avformat_find_stream_info(NULL)
, ptr_av_seek_frame(NULL)
, ptr_av_read_frame(NULL)
, ptr_av_free_packet(NULL)
, ptr_av_bitstream_filter_init(NULL)
, ptr_av_bitstream_filter_filter(NULL)
, ptr_av_bitstream_filter_close(NULL)
, ptr_avcodec_register_all(NULL)
, ptr_av_free(NULL)
{
	//
}

FFMpegStub::~FFMpegStub()
{
	//
}

int FFMpegStub::Initialize(const char *base_dir /* = NULL */)
{
	// 组合动态库路径
	std::string dir = base_dir;
	
	std::string avformat = dir;
	avformat += LIB_AVFORMAT;

	std::string avcodec = dir;
	avcodec += LIB_AVCODEC;

	std::string avutil = dir;
	avutil += LIB_AVUTIL;

	std::string avfilter = dir;
	avfilter += LIB_AVFILTER;

	std::string avdevice = dir;
	avdevice += LIB_AVDEVICE;

	std::string postproc = dir;
	postproc += LIB_POSTPROC;

	std::string swresample = dir;
	swresample += LIB_SWRESAMPLE;

	std::string swscale = dir;
	swscale += LIB_SWSCALE;

	// 开始加载各种动态库，加载顺序有一定的技巧
	// 先加载没有相互依赖的库
	// 1. avutil
	// 2. swresample
	// 3. avcodec
	// 4. avformat
	// 5. postproc
	// 6. swscale
	// 7. avdevice
	// 8. avfilter

	h_avutil_		= LoadLibraryA(avutil.c_str());
	h_swresample_	= LoadLibraryA(swresample.c_str());
	h_avcodec_		= LoadLibraryA(avcodec.c_str());
	h_avformat_		= LoadLibraryA(avformat.c_str());
	h_postproc_		= LoadLibraryA(postproc.c_str());
	h_swscale_		= LoadLibraryA(swscale.c_str());
	h_avdevice_		= LoadLibraryA(avdevice.c_str());
	h_avfilter_		= LoadLibraryA(avfilter.c_str());

	// 开始加载函数

	// avformat
	ptr_av_register_all				= (_av_register_all)GetProcAddress((HMODULE)h_avformat_, "av_register_all");
	ptr_avformat_network_init		= (_avformat_network_init)GetProcAddress((HMODULE)h_avformat_, "avformat_network_init");
	ptr_avformat_open_input			= (_avformat_open_input)GetProcAddress((HMODULE)h_avformat_, "avformat_open_input");
	ptr_avformat_close_input		= (_avformat_close_input)GetProcAddress((HMODULE)h_avformat_, "avformat_close_input");
	ptr_avformat_find_stream_info	= (_avformat_find_stream_info)GetProcAddress((HMODULE)h_avformat_, "avformat_find_stream_info");
	ptr_av_seek_frame				= (_av_seek_frame)GetProcAddress((HMODULE)h_avformat_, "av_seek_frame");
	ptr_av_read_frame				= (_av_read_frame)GetProcAddress((HMODULE)h_avformat_, "av_read_frame");
	
	
	// avcodec
	ptr_av_free_packet				= (_av_free_packet)GetProcAddress((HMODULE)h_avcodec_, "av_free_packet");
	ptr_av_bitstream_filter_init	= (_av_bitstream_filter_init)GetProcAddress((HMODULE)h_avcodec_, "av_bitstream_filter_init");
	ptr_av_bitstream_filter_filter	= (_av_bitstream_filter_filter)GetProcAddress((HMODULE)h_avcodec_, "av_bitstream_filter_filter");
	ptr_av_bitstream_filter_close	= (_av_bitstream_filter_close)GetProcAddress((HMODULE)h_avcodec_, "av_bitstream_filter_close");
	ptr_avcodec_register_all		= (_avcodec_register_all)GetProcAddress((HMODULE)h_avcodec_, "avcodec_register_all");

	// avutil
	ptr_av_free						= (_av_free)GetProcAddress((HMODULE)h_avutil_, "av_free");

	// 准备完毕
	// 立即初始化一下FFMpeg环境
	ptr_av_register_all();
	ptr_avformat_network_init();

	return 0;
}

void FFMpegStub::Destroy()
{
	FreeLibrary((HMODULE)h_avfilter_);
	FreeLibrary((HMODULE)h_avdevice_);
	FreeLibrary((HMODULE)h_swscale_);
	FreeLibrary((HMODULE)h_postproc_);
	FreeLibrary((HMODULE)h_avformat_);
	FreeLibrary((HMODULE)h_avcodec_);
	FreeLibrary((HMODULE)h_swresample_);
	FreeLibrary((HMODULE)h_avutil_);
}