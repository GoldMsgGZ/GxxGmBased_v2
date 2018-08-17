#ifndef _PSPackager_H_
#define _PSPackager_H_

#include "PSFormat.h"

class PSPackager
{
public:
	PSPackager();
	~PSPackager();

public:
	int Open(EnumGSPSCodecType video_codec_id, EnumGSPSCodecType audio_codec_id);
	void Close();

	int Push();

public:
	static void CALLBACK _PSDataReceivedCallBack(GS_MpegPSHandle handle, StruPSFrameInfo const* stPSFrameInfo, void* pUserData);

private:
	GS_MpegPSHandle handle_;
};

#endif
