#include <iostream>
#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>

#include "..\GxxGmGoVideo\GxxGmGoVideo.h"
//#include "GSRtspClient.h"

int _tmain(int argc, const TCHAR *argv[])
{
	if (argc < 3)
	{
		return 0;
	}

	char str_govideo_ip[4096] = {0};
	char str_govideo_port[4096] = {0};

	strcpy_s(str_govideo_ip, 4096, argv[1]);
	strcpy_s(str_govideo_port, 4096, argv[2]);

	unsigned short govideo_port = atoi(str_govideo_port);

	// 首先，登录到GoVideo
	GxxGmGoVideo govideo;
	govideo.Initialize();
	//system("pause");
	//govideo.Destroy();
	Sleep(10);

	int errCode = govideo.Login(str_govideo_ip, govideo_port, "cyfid111", "cyfid111", 107, "ad22min", "admin2222");
	if (errCode != 0)
	{
		printf("登录到GoVideo失败！错误码：%d\n", errCode);
		return errCode;
	}

	// 获取所有设备信息
	errCode = govideo.GetAllDevices();
	if (errCode != 0)
	{
		printf("获取所有设备信息失败！错误码：%d\n", errCode);
		return errCode;
	}

	// 获取所有设备状态信息
	errCode = govideo.GetAllDeviceStatus();
	if (errCode != 0)
	{
		printf("获取所有设备状态信息失败！错误码：%d\n", errCode);
		return errCode;
	}

	// 订阅消息
	errCode = govideo.SubscriptionMsg(0, "10.10.2.102", 9900);
	if (errCode != 0)
	{
		printf("订阅GoVideo消息失败！错误码：%d\n", errCode);
		return errCode;
	}

	// 打印所有设备在线状态信息
	std::vector<GOVIDEO_DEVICE_STATUS>::iterator iter;
	for (iter = govideo.devices_status_.begin(); iter != govideo.devices_status_.end(); ++iter)
	{
		GOVIDEO_DEVICE_STATUS *dev_status = &(*iter);
		printf("设备国标编码：%s 内部设备ID：%d 设备在线状态：%d\n", dev_status->gb28181_code_.c_str(), dev_status->device_id_, dev_status->status_);
	}
	
	// 然后尝试发送点流命令
	std::string url;
	errCode = govideo.GetRealStreamByGBCode("44010401901511000001", url);
	if (errCode != 0)
		printf("点流失败！错误码：%d\n", errCode);
	else
		printf("点流成功！URL为：%s\n", url.c_str());

	// 这里是否搞一个RTSP客户端来拉流

	system("pause");
	return 0;
}