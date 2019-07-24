// GxxGmOCXCheck.cpp : �������̨Ӧ�ó������ڵ㡣
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

	// 1. ���ȼ�����Ƿ��Ѿ���װ
	// 1.1. ��� HKMC\\SOFTWARE\\MozillaPlugins\\@gosun.com/npGSVideoPlugin ���Ƿ���Path�ֶΣ�ֵΪ����
	//      Ȼ�����·���Ƿ���ڸò��
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
		// �������key����ȡ·��
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

		// ����ļ��Ƿ����
		DWORD file_attr = GetFileAttributes((TCHAR*)buffer);
		if (file_attr == INVALID_FILE_ATTRIBUTES)
		{
			errCode = GetLastError();
			delete [] buffer;
			return errCode;
		}
		else
		{
			std::cout<<"�Ѱ�װ�����˹���OCX���ſؼ�"<<std::endl;
		}

		delete [] buffer;
		
	}

	// 1.2. �������OCX�ؼ��������Ƿ��������ָ���Ŀؼ�

	// 2. ���Chromeλ��
	// 2.1. ��鵱ǰϵͳ��32λ����64λ
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
			// ���Դ򿪽���
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
						std::cout<<"��32λӦ�ó��򣬷���Ҫ��"<<std::endl;
					}
					else
					{
						std::cout<<"��64λӦ�ó��򣬲�����Ҫ��"<<std::endl;
					}
				}
			}
		}
		
	} while (Process32Next(hSnapshot, &pe32));


	// 2.2. �����64λ�ģ����ж������е�Chrome��32λ����64λ


	// 3. ���д���ı�
	return 0;
}

