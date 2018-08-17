#include "stdafx.h"
#include "GxxGmInputDevice.h"
#include <windows.h>

#pragma comment(lib, "Strmiids.lib")

//////////////////////////////////////////////////////////////////////////
//
//

GxxGmDevice::GxxGmDevice()
: observer_(NULL)
, device_read_thread_exit_(true)
, recode_thread_exit_(true)
, input_format_ctx_(NULL)
, input_video_stream_id_(-1)
, input_audio_stream_id_(-1)
{

}

GxxGmDevice::~GxxGmDevice()
{

}

int GxxGmDevice::InsertObserver(GxxGmDeviceObserver *observer)
{
	observer_ = observer;

	if (!observer_)
		return -1;
	else
		return 0;
}

void  GxxGmDevice::RemoveObserver()
{
	observer_ = NULL;
}

int GxxGmDevice::Open(const char *device_name, int device_type/* = 0 */)
{
	int errCode = 0;
	char errmsg[4096] = {0};

	device_type_ = device_type;
	device_name_ = device_name;

	return errCode;
}

int GxxGmDevice::Start()
{
	char dbgmsg[4096] = {0};
	int errCode = 0;

	if (device_read_thread_.isRunning())
		return -1;
	
	if (recode_thread_.isRunning())
		return -2;

	// 启动
	try
	{
		device_read_thread_exit_ = false;
		device_read_thread_.start(device_read_thread_fun, this);
		Sleep(10);
	}
	catch (Poco::Exception e)
	{
		std::string estr = e.displayText();
		int errCode = e.code();
		sprintf_s(dbgmsg, 4096, "[设备对象] 名称：%s 启动线程 device_read_thread_ 出现异常。异常信息：%s，异常代码：%d\n", device_name_, estr.c_str(), errCode);
		OutputDebugStringA(dbgmsg);
		return errCode;
	}
	

	//recode_thread_exit_ = false;
	//recode_thread_.start(GxxGmDevice::recode_thread_fun, this);
	//Sleep(10);

	try
	{
		observer_dispatch_thread_exit_ = false;
		observer_dispatch_thread_.start(observer_dispatch_thread_fun, this);
		Sleep(10);
	}
	catch (Poco::Exception e)
	{
		std::string estr = e.displayText();
		int errCode = e.code();
		sprintf_s(dbgmsg, 4096, "[设备对象] 名称：%s 启动线程 observer_dispatch_thread_ 出现异常。异常信息：%s，异常代码：%d\n", device_name_, estr.c_str(), errCode);
		OutputDebugStringA(dbgmsg);
		return errCode;
	}
	
	return errCode;
}

void GxxGmDevice::Stop()
{
	device_read_thread_exit_ = true;
	recode_thread_exit_ = true;

	device_read_thread_.join();
	recode_thread_.join();
}

void GxxGmDevice::Close()
{
	Stop();
	avformat_close_input(&input_format_ctx_);
	input_format_ctx_ = NULL;
}

void GxxGmDevice::device_read_thread_fun(void *param)
{
	GxxGmDevice *device = (GxxGmDevice*)param;
	int errCode = 0;
	char errmsg[4096] = {0};

	device->input_format_ctx_ = avformat_alloc_context();
	device->iformat_ = av_find_input_format("dshow");

	char device_symbolink[128] = {0};
	if (device->device_type_ == 0)
		sprintf_s(device_symbolink, 128, "video=%s", device->device_name_.c_str());
	else if (device->device_type_ == 1)
		sprintf_s(device_symbolink, 128, "audio=%s", device->device_name_.c_str());

	errCode = avformat_open_input(&device->input_format_ctx_, device_symbolink, device->iformat_, NULL);
	if (errCode < 0)
	{
		av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
		return ;
	}

	errCode = avformat_find_stream_info(device->input_format_ctx_, NULL);
	if (errCode < 0)
	{
		av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
		//avformat_close_input(&input_format_ctx_);
		return ;
	}

	int streams = device->input_format_ctx_->nb_streams;
	for (int index = 0; index < streams; ++index)
	{
		if (device->input_format_ctx_->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			// 视频流信息
			device->input_video_stream_id_ = index;

			// 找解码器
			device->video_decoder_ = avcodec_find_decoder(device->input_format_ctx_->streams[index]->codec->codec_id);
			if (device->video_decoder_ == NULL)
			{
				// 
				return ;
			}

			device->video_decoder_ctx_ = avcodec_alloc_context3(device->video_decoder_);
			if (device->video_decoder_ctx_ == NULL)
			{
				return ;
			}

			errCode = avcodec_copy_context(device->video_decoder_ctx_, device->input_format_ctx_->streams[index]->codec);
			if (errCode < 0)
			{
				av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
				return ;
			}

			// 打开解码器
			avcodec_open2(device->video_decoder_ctx_, device->video_decoder_, NULL);
		}
		else if (device->input_format_ctx_->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			// 音频流信息
			device->input_audio_stream_id_ = index;
		}
	}

	try
	{
		device->recode_thread_exit_ = false;
		device->recode_thread_.start(GxxGmDevice::recode_thread_fun, device);
		Sleep(10);
	}
	catch (Poco::Exception e)
	{
		std::string estr = e.displayText();
		int errCode = e.code();
		sprintf_s(errmsg, 4096, "[设备对象] 名称：%s 启动线程 recode_thread_ 出现异常。异常信息：%s，异常代码：%d\n", device->device_name_, estr.c_str(), errCode);
		OutputDebugStringA(errmsg);
		return ;
	}

	AVPacket pkt;
	while (!device->device_read_thread_exit_)
	{
		errCode = av_read_frame(device->input_format_ctx_, &pkt);
		if (errCode < 0)
		{
			av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
			break;
		}

		if (pkt.stream_index == device->input_video_stream_id_)
		{
			// 视频编码包，扔进视频编码包队列
			device->original_video_packet_queue_.push(pkt);
		}
		else if (pkt.stream_index == device->input_audio_stream_id_)
		{
			// 视频编码包，扔进视频编码包队列
			device->original_audio_packet_queue_.push(pkt);
		}
	}

	// 流搞完了
}

void GxxGmDevice::recode_thread_fun(void *param)
{
	GxxGmDevice *device = (GxxGmDevice*)param;
	char errmsg[4096] = {0};
	int errCode = 0;

	// 准备编码器
	if (device->video_decoder_ctx_->codec_id != AV_CODEC_ID_H264)
	{
		// 我们需要重编码为H.264
		device->video_encoder_ = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (device->video_encoder_ == NULL)
		{
			return ;
		}

		device->video_encoder_ctx_= avcodec_alloc_context3(device->video_encoder_);
		//device->video_encoder_ctx_->compression_level = 5;
		device->video_encoder_ctx_->width = device->video_decoder_ctx_->width;
		device->video_encoder_ctx_->height = device->video_decoder_ctx_->height;
		device->video_encoder_ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
		//device->video_encoder_ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
		device->video_encoder_ctx_->time_base.num = 1;
		device->video_encoder_ctx_->time_base.den = 30;
		device->video_encoder_ctx_->bit_rate = device->video_decoder_ctx_->bit_rate;

		av_opt_set(device->video_encoder_ctx_->priv_data, "preset", "slow", 0);
		av_opt_set(device->video_encoder_ctx_->priv_data, "tune", "zerolatency", 0);

		// 打开编码器
		errCode = avcodec_open2(device->video_encoder_ctx_, device->video_encoder_, NULL);
		if (errCode < 0)
		{
			av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
			return ;
		}
	}
	
	while (!device->recode_thread_exit_)
	{
		// 取视频编码帧，用阻塞读取方法？可以先试试
		AVPacket *video_pkt = NULL;
		device->original_video_packet_queue_.front(&video_pkt, true);

		// 如果是H.264编码的视频，那我们干脆直接扔出去好了
		AVFrame *frame = av_frame_alloc();
		int got_pic = 0;
		int errCode = avcodec_decode_video2(device->video_decoder_ctx_, frame, &got_pic, video_pkt);
		if (errCode < 0)
		{
			av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
			av_frame_free(&frame);
			av_free_packet(video_pkt);
			continue;
		}

		if (!got_pic)
		{
			av_frame_free(&frame);
			av_free_packet(video_pkt);
			continue;
		}

		// 重新编码
		AVPacket new_video_pkt;
		new_video_pkt.buf = NULL;
		new_video_pkt.size = 0;
		av_init_packet(&new_video_pkt);
		
		int got_pkt = 0;
		errCode = avcodec_encode_video2(device->video_encoder_ctx_, &new_video_pkt, frame, &got_pkt);
		if (errCode < 0)
		{
			av_make_error_string(errmsg, AV_ERROR_MAX_STRING_SIZE, errCode);
			av_frame_free(&frame);
			av_free_packet(video_pkt);
			continue;
		}

		if (!got_pkt)
		{
			av_frame_free(&frame);
			av_free_packet(video_pkt);
			continue;
		}

		// 出队
		device->original_video_packet_queue_.pop();
		av_free_packet(video_pkt);

		// 新的编码包扔进队列，准备传输
		device->recode_packet_queue_.push(new_video_pkt);
	}
}

void GxxGmDevice::observer_dispatch_thread_fun(void *param)
{
	GxxGmDevice *device = (GxxGmDevice*)param;

	while (!device->observer_dispatch_thread_exit_)
	{
		AVPacket *video_pkt = NULL;
		int errCode = device->recode_packet_queue_.front(&video_pkt, true);
		if (errCode != 0)
		{
			av_free_packet(video_pkt);
			continue;
		}

		device->observer_->MediaData(video_pkt->buf->data, video_pkt->size);
		TRACE("发送了一个视频帧\n");
		av_free_packet(video_pkt);
	}
}


//////////////////////////////////////////////////////////////////////////
//
//

GxxGmInputDevice::GxxGmInputDevice()
{
	av_register_all();
	avformat_network_init();
	avdevice_register_all();
	avcodec_register_all();
}

GxxGmInputDevice::~GxxGmInputDevice()
{

}

int GxxGmInputDevice::EnumDevices()
{
	int errCode = 0;

	EnumInputDevices(&video_devices, CLSID_VideoInputDeviceCategory);
	EnumInputDevices(&audio_devices, CLSID_AudioInputDeviceCategory);

	return errCode;
}

int GxxGmInputDevice::EnumInputDevices(std::map<int, TDeviceName> *devices, REFGUID guidValue)
{
	int errCode = 0;

	TDeviceName name;   
	HRESULT hr;  

	// 初始化  
	devices->clear();  

	// 初始化COM  
	hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );  
	if (FAILED(hr))  
	{  
		return hr;  
	}  

	// 创建系统设备枚举器实例  
	ICreateDevEnum *pSysDevEnum = NULL;  
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);  
	if (FAILED(hr))  
	{  
		CoUninitialize();  
		return hr;  
	}  

	// 获取设备类枚举器  
	int index = 0;
	IEnumMoniker *pEnumCat = NULL;  
	hr = pSysDevEnum->CreateClassEnumerator(guidValue, &pEnumCat, 0);  
	if (hr == S_OK)   
	{  
		// 枚举设备名称  
		IMoniker *pMoniker = NULL;  
		ULONG cFetched;  
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)  
		{  
			IPropertyBag *pPropBag;  
			hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropBag);  
			if (SUCCEEDED(hr))  
			{  
				// 获取设备友好名  
				VARIANT varName;  
				VariantInit(&varName);  

				hr = pPropBag->Read(L"FriendlyName", &varName, NULL);  
				if (SUCCEEDED(hr))  
				{  
					StringCchCopy(name.FriendlyName, MAX_FRIENDLY_NAME_LENGTH, varName.bstrVal);  

					// 获取设备Moniker名  
					LPOLESTR pOleDisplayName = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(MAX_MONIKER_NAME_LENGTH * 2));  
					if (pOleDisplayName != NULL)  
					{  
						hr = pMoniker->GetDisplayName(NULL, NULL, &pOleDisplayName);  
						if (SUCCEEDED(hr))  
						{  
							StringCchCopy( name.MonikerName, MAX_MONIKER_NAME_LENGTH, pOleDisplayName );  
							devices->insert(std::pair<int, TDeviceName>(index, name));
							++index;
						}  

						CoTaskMemFree( pOleDisplayName );  
					}  
				}  

				VariantClear( &varName );  
				pPropBag->Release();                       
			}  

			pMoniker->Release();  
		} // End for While  

		pEnumCat->Release();  
	}  

	pSysDevEnum->Release();  
	CoUninitialize();

	return errCode;
}