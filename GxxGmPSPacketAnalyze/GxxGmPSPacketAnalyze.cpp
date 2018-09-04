// GxxGmPSPacketAnalyze.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PSFormat.h"
#include <iostream>
#include <Windows.h>


static bool finish = false;

void CALLBACK _ESFrameReceivedCallBack(GS_MpegPSHandle handle, StruESFrameInfo const *stESFrameInfo, StruPSFrameInfo const* stPSFrameInfo, void* pUserData)
{
	std::cout<<"解析PS包..."<<std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char ps_file[4096] = {0};
	strcpy_s(ps_file, 4096, argv[1]);

	// 读取数据
	int errCode = 0;
	HANDLE hPSFile = CreateFileA(ps_file, GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPSFile == INVALID_HANDLE_VALUE)
	{
		errCode = GetLastError();
		return errCode;
	}

	DWORD file_size_h = 0;
	DWORD file_size_l = GetFileSize(hPSFile, &file_size_h);

	unsigned char *data = new unsigned char[file_size_l];
	DWORD readed = 0;
	BOOL bret = ReadFile(hPSFile, data, file_size_l, &readed, NULL);
	if (!bret)
	{
		errCode = GetLastError();
		CloseHandle(hPSFile);
		return errCode;
	}

	CloseHandle(hPSFile);

	GS_MpegPSHandle handle = NULL;
	EnumGS_MPEGPS_RetCode err = GS_MPEGPS_CreatePS2ESHandle(&handle);

	// 将数据扔进去，等待回调
	err = GS_MPEGPS_SetESFrameReceivedCallback(handle, _ESFrameReceivedCallBack, NULL);

	err = GS_MPEGPS_InputPSData(handle, data, file_size_l);

	while (!finish)
	{
		Sleep(1000);
	}
	
	GS_MPEGPS_DestoryPS2ESHandle(handle);

	system("pause");
	return 0;
}

