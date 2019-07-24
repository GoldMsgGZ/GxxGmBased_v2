// GxxGmOCXCheck.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>

#pragma comment(lib, "Advapi32.lib")

#define subkey	_T("SOFTWARE\\MozillaPlugins\\@gosun.com/npGSVideoPlugin")
#define val		_T("Path")

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

int _tmain(int argc, _TCHAR* argv[])
{
	int errCode = 0;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	// 1. 首先检查插件是否已经安装
	// 1.1. 检测 HKMC\\SOFTWARE\\MozillaPlugins\\@gosun.com/npGSVideoPlugin 下是否有Path字段，值为多少
	//      然后检查该路径是否存在该插件
	HKEY hKey = NULL;
	DWORD dwDisposition = 0;
	LONG ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, NULL, 0, KEY_READ, NULL, &hKey, &dwDisposition);
	if (ret != ERROR_SUCCESS)
	{
		errCode = GetLastError();
		return errCode;
	}

	if (dwDisposition == REG_OPENED_EXISTING_KEY)
	{
		// 存在这个key，读取路径
		DWORD dwType = 0;
		DWORD buffer_len = 1024;
		unsigned char *buffer = new unsigned char[buffer_len];
		
		while (true)
		{
			ret = RegGetValue(HKEY_LOCAL_MACHINE, subkey, val, RRF_RT_REG_SZ, &dwType, buffer, &buffer_len);
			if (ret == ERROR_SUCCESS)
			{
				break;
			}
			else if (ret == ERROR_MORE_DATA)
			{
				delete [] buffer;
				buffer = new unsigned char[buffer_len];
				continue;
			}
			else
			{
				errCode = GetLastError();
				delete [] buffer;
				return errCode;
			}
		}

		// 检查文件是否存在
		DWORD file_attr = GetFileAttributes((TCHAR*)buffer);
		if (file_attr == INVALID_FILE_ATTRIBUTES)
		{
			errCode = GetLastError();
			delete [] buffer;
			return errCode;
		}
		else
		{
			std::cout<<"已安装高新兴国迈OCX播放控件"<<std::endl;
		}

		delete [] buffer;
		
	}

	// 1.2. 检测所有OCX控件，看看是否存在我们指定的控件

	// 2. 检测Chrome位数
	// 2.1. 检查当前系统是32位还是64位
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		errCode = GetLastError();
		return errCode;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bret = Process32First(hSnapshot, &pe32);
	if (!bret)
	{
		errCode = GetLastError();
		return errCode;
	}

	do 
	{
		TCHAR processName[4096] = {0};
		_tcscpy_s(processName, 4096, pe32.szExeFile);
		//_tcslwr_s(processName, 4096);

		if (_tcsicmp(processName, _T("chrome.exe")) == 0)
		{
			// 尝试打开进程
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
			if (hProcess == NULL)
			{
				errCode = GetLastError();
				continue;
			}
			else
			{
				if (fnIsWow64Process != NULL)
				{
					BOOL bIsWow64 = FALSE;
					bret = fnIsWow64Process(hProcess, &bIsWow64);
					if (bIsWow64)
					{
						std::cout<<"是32位应用程序，符合要求"<<std::endl;
					}
					else
					{
						std::cout<<"是64位应用程序，不符合要求"<<std::endl;
					}
				}
			}
		}
		
	} while (Process32Next(hSnapshot, &pe32));


	// 2.2. 如果是64位的，则判断运行中的Chrome是32位还是64位


	// 3. 结果写入文本
	return 0;
}

