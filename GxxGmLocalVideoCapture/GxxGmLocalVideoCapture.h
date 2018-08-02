#ifndef _GxxGmLocalVideoCapture_H_
#define _GxxGmLocalVideoCapture_H_

#include <windows.h>  
#include <vector>  
#include <dshow.h>  

#ifndef MACRO_GROUP_DEVICENAME  
#define MACRO_GROUP_DEVICENAME  

#define MAX_FRIENDLY_NAME_LENGTH    128  
#define MAX_MONIKER_NAME_LENGTH     256  

typedef struct _TDeviceName  
{  
	WCHAR FriendlyName[MAX_FRIENDLY_NAME_LENGTH];   // 设备友好名  
	WCHAR MonikerName[MAX_MONIKER_NAME_LENGTH];     // 设备Moniker名  
} TDeviceName;  
#endif

class GxxGmLocalVideoCapture
{
public:
	GxxGmLocalVideoCapture();
	~GxxGmLocalVideoCapture();

public:
	int EnumVideoDevices();

public:
	int StartCapture(int video_index);
	int StopCapture();

public:
	std::map<int, TDeviceName> video_devices;
};

#endif//_GxxGmLocalVideoCapture_H_
