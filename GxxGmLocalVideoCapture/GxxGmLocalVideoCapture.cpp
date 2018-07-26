#include "GxxGmLocalVideoCapture.h"

GxxGmLocalVideoCapture::GxxGmLocalVideoCapture()
{

}

GxxGmLocalVideoCapture::~GxxGmLocalVideoCapture()
{

}

int GxxGmLocalVideoCapture::EnumVideoDevices()
{
	// 这里枚举所有本地摄像头
	int errCode = 0;

	return errCode;
}

int GxxGmLocalVideoCapture::StartCapture(int video_index)
{
	// 这里开始进行视频采集，我们应该会收到摄像头返回的编码包
	// 需要一个回调接口将编码包传出去
	int errCode = 0;

	return errCode;
}

int GxxGmLocalVideoCapture::StopCapture()
{
	return 0;
}