#include "stdafx.h"
#include "GB28181SimulateDevice.h"
#include "GSRtpServer.h"

GB28181SimulateDevice::GB28181SimulateDevice()
: camera_(new RealDeviceObject(this))
, mic_(new RealDeviceObject(this))
, rtp_stream_mgr_(new RtpStreamMgr())
{

}

GB28181SimulateDevice::~GB28181SimulateDevice()
{
	delete camera_;
	delete mic_;
	delete rtp_stream_mgr_;
}

int GB28181SimulateDevice::SetDeviceGbcode(const char *device_gbcode)
{
	device_gbcode_ = device_gbcode;
	return 0;
}

int GB28181SimulateDevice::SetVideoGbcode(const char *video_gbcode)
{
	video_gbcode_ = video_gbcode;
	return 0;
}

int GB28181SimulateDevice::SetVideoDeviceName(const char *device_name)
{
	return camera_->SetDeviceName(device_name);
}

int GB28181SimulateDevice::SetAudioDeviceName(const char *device_name)
{
	return mic_->SetDeviceName(device_name);
}

int GB28181SimulateDevice::GetDeviceCatlog(int &catlog_counts, StruCatalogInfo **catlogs)
{
	int errCode = 0;

	catlog_counts = 1;

	*catlogs = new StruCatalogInfo[catlog_counts];
	ZeroMemory(&(*catlogs)[0], sizeof(StruCatalogInfo));
	strncpy((*catlogs)[0].czGBCode, video_gbcode_.c_str(), STR_GBCODE_LEN);
	strncpy((*catlogs)[0].czDeviceName, "高新兴国迈模拟摄像头通道", STR_NAME_LEN);
	strncpy((*catlogs)[0].czModel, "GXXGM-SIM-VIDEO", STR_MODEL_LEN);
	strncpy((*catlogs)[0].czManufacturer, "高新兴国迈", STR_MANUFACTURER_LEN);
	strncpy((*catlogs)[0].czOwner, "高新兴国迈", STR_OWNER_LEN);
	strncpy((*catlogs)[0].czCivilCode, "44000000", STR_CIVILCODE_LEN);
	strncpy((*catlogs)[0].czBlock, "44000000", STR_BLOCK_LEN);
	strncpy((*catlogs)[0].czAddress, "44000000", STR_ADDRESS_LEN);
	(*catlogs)[0].iParental = 1;
	strncpy((*catlogs)[0].czParentID, device_gbcode_.c_str(), STR_PARENTID_LEN);
	(*catlogs)[0].iSafetyWay = 0;
	(*catlogs)[0].iRegisterWay = 1;
	strncpy((*catlogs)[0].czCertNum, "CertNum 0" , STR_CERTNUM_LEN);
	(*catlogs)[0].iCertifiable = 0;
	(*catlogs)[0].iErrCode = 400;
	strncpy((*catlogs)[0].czEndTime, " 2010-11-11T19:46:17", STR_DATETIME_LEN);
	(*catlogs)[0].iSecrecy = 0;
	strncpy((*catlogs)[0].czIPAddress, "192.168.199.102 ", STR_IPADDRESS_LEN);
	(*catlogs)[0].iPort = 5060;
	strncpy((*catlogs)[0].czPassword, "Password 0", STR_PASSWORD_LEN);
	(*catlogs)[0].iStatus = 1;
	(*catlogs)[0].dLongitude = 171.3;
	(*catlogs)[0].dLatitude = 34.2;
	(*catlogs)[0].iPtzType = 2;
	(*catlogs)[0].iRoomType = 3;

	return errCode;
}

int GB28181SimulateDevice::GetDeviceInfo(StruDeviceInfo *device_info)
{
	int errCode = 0;

	device_info->bResultOK = true;
	strncpy(device_info->czDeviceName, "高新兴国迈模拟摄像头01", STR_NAME_LEN);
	strncpy(device_info->czManufacturer, "GOSUNCN", STR_MANUFACTURER_LEN);
	strncpy(device_info->czModel, "GXX-GM-SIMULATE-001", STR_MODEL_LEN);
	strncpy(device_info->czFirmware, "V1.0, build 0001", STR_FIREWARE_LEN);
	device_info->iChanal = 1;

	return errCode;
}

int GB28181SimulateDevice::GetDeviceStatus(StruDeviceStatus *device_status)
{
	int errCode = 0;

	SYSTEMTIME st;
	GetLocalTime(&st);
	char device_datetime[STR_DATETIME_LEN] = {0};
	sprintf_s(device_datetime, STR_DATETIME_LEN, "%d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	strncpy(device_status->czDevDateTime, device_datetime, STR_DATETIME_LEN);
	strncpy(device_status->czErrReason, "NO ERROR", STR_ERRREASON_LEN);
	device_status->bResult = true;
	device_status->bOnLine = true;
	device_status->bRecord = false;
	device_status->bEncode = false;
	device_status->bStatusOK = true;
	device_status->iAlarmNum = 1;

	StruAlarmStatus * stuAlarm = new StruAlarmStatus;
	ZeroMemory(stuAlarm, sizeof(StruAlarmStatus));
	strncpy(stuAlarm->czAlarmID, device_gbcode_.c_str(), STR_ALARMID_LEN);	// 报警设备编码
	stuAlarm->eStatus = eONDUTY;

	device_status->ptrAlarm = stuAlarm;

	return errCode;
}

int GB28181SimulateDevice::GetDeviceRecordInfo(const char *start_time, const char *end_time, StruRecordIndex *record_info)
{
	int errCode = 0;

	strncpy(record_info->czName, "Cameral", STR_NAME_LEN);
	record_info->iSumNum = 2;		// 两个录像

	//////////////////////////////////////////////////////////////////////////
	//
	// 组织录像信息
	StruRecordPeriod * ary = new StruRecordPeriod[2];

	ZeroMemory(&ary[0], sizeof(StruRecordPeriod));
	strncpy(ary[0].czDeviceID, device_gbcode_.c_str(), STR_GBCODE_LEN);
	strncpy(ary[0].czName, "Camera1 ", STR_NAME_LEN);
	strncpy(ary[0].czFilePath, "64010000002100000001", STR_FILEPATH_LEN);
	strncpy(ary[0].czAddress, "Address 1", STR_ADDRESS_LEN);
	strncpy(ary[0].czRecorderID, "44000000001320000001", STR_GBCODE_LEN);
	strncpy(ary[0].czStartDT, "2017-07-05T09:10:00", STR_DATETIME_LEN);
	strncpy(ary[0].czEndDT, "2017-07-05T09:14:00", STR_DATETIME_LEN);
	strncpy(ary[0].czType, "time", STR_TYPE_LEN);
	ary[0].iSecrecy = 1;

	ZeroMemory(&ary[1], sizeof(StruRecordPeriod));
	strncpy(ary[1].czDeviceID, device_gbcode_.c_str(), STR_GBCODE_LEN);
	strncpy(ary[1].czName, "Camera2 ", STR_NAME_LEN);
	strncpy(ary[1].czFilePath, "34010000002100000001  ", STR_FILEPATH_LEN);
	strncpy(ary[1].czAddress, "Address 2", STR_ADDRESS_LEN);
	strncpy(ary[1].czRecorderID, "44000000001320000002", STR_GBCODE_LEN);
	strncpy(ary[1].czStartDT, "2017-07-06T11:20:00", STR_DATETIME_LEN);
	strncpy(ary[1].czEndDT, "2017-07-06T11:24:00", STR_DATETIME_LEN);
	strncpy(ary[1].czType, "time", STR_TYPE_LEN);
	ary[1].iSecrecy = 0;

	record_info->ptrPeroid = ary;

	return errCode;
}

int GB28181SimulateDevice::AddRealStream(STREAM_HANDLE streamHandle, int iSSRC, unsigned short & iLocalPort)
{
	int errCode = 0;

	// 首先计算Token
	char current_token[32] = {0};
	memset(current_token, 0, 32);
	sprintf_s(current_token, 32, "%d", streamHandle);

	//SSRC_ = iSSRC;

	errCode = GSRTPServer_AddSource(current_token, iSSRC, &iLocalPort);
	if (errCode == GSRTP_SUCCESS)
	{
		rtp_stream_mgr_->AddStream(streamHandle, iSSRC, RtpStream_Real);
	}

	return errCode;
}

int GB28181SimulateDevice::StartRealStream(STREAM_HANDLE streamHandle, StruMediaInfo *remote_media_info)
{
	StruRtpAVSampleRate rtp_av_samplerate;
	rtp_av_samplerate.iAudioSampleRate = 8000;
	rtp_av_samplerate.iVideoSampleRate = 90000;

	char current_token[32] = {0};
	memset(current_token, 0, 32);
	sprintf_s(current_token, 32, "%d", streamHandle);

	// 从map中找到对应的流数据
	RtpStreamInfo *element = rtp_stream_mgr_->GetStreamInfo(streamHandle);
	if (element == NULL)
	{
		// 没找到
		return -1;
	}
	
	const char *token = element->stream_token_;
	int ssrc = element->SSRC_;

	GSRTP_ERR err = GSRTPServer_SetSourceParam(token, ssrc, remote_media_info->czIP, remote_media_info->iPort, &rtp_av_samplerate, 3*1024*1024);
	if (err != GSRTP_SUCCESS)
	{
		rtp_stream_mgr_->RemoveStream(streamHandle);
		GSRTPServer_Reclaim(token, ssrc);
		return err;
	}

	// 启动真实设备，开始取流
	camera_->OpenRealDevice();
	camera_->Start(streamHandle, ssrc, AV_CODEC_ID_H264);

	mic_->OpenRealDevice();
	mic_->Start(streamHandle, ssrc, AV_CODEC_ID_PCM_MULAW);

	return err;
}

int GB28181SimulateDevice::StopStream(STREAM_HANDLE streamHandle)
{
	camera_->is_stream_handler_stop_ = true;
	camera_->stream_handler_.join();

	rtp_stream_mgr_->RemoveStream(streamHandle);
	return 0;
}

int GB28181SimulateDevice::MediaData(void *stream_handle, int ssrc, StruPSFrameInfo *ps_frame)
{
	char current_token[32] = {0};
	memset(current_token, 0, 32);
	sprintf_s(current_token, 32, "%d", stream_handle);

	StruRtpFrame rtp_frame;
	rtp_frame.eFrameType = RTP_FRAME_PS;
	rtp_frame.iCodeID = 0x00000400;
	rtp_frame.iTimeStamp = ps_frame->nPTS;
	rtp_frame.iSSRC = ssrc;
	rtp_frame.pFrame = (char*)ps_frame->pBuffer;
	rtp_frame.iLenght = ps_frame->nBufLen;
	GSRTP_ERR err = GSRTPServer_InputStream(current_token, &rtp_frame);
	return err;
}