#include "GxxGmService.h"
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <Tlhelp32.h>

const TCHAR *apps[] = {
	_T("ALMon.exe"),				// ɱ��&������
	_T("ALsvc.exe"),				// ɱ��&������ 
	_T("ManagementAgentNT.exe"),	// ɱ��&������
	_T("RouterNT.exe"),				// ɱ��&������
	_T("ssp.exe"),					// ɱ��&������
	_T("swc_service.exe"),			// ɱ��&������
	_T("SophosEndPoint+EXP.exe"),	// ɱ��&������
	_T("hmpalert.exe"),				// ɱ��&������
	_T("winrdlv3.exe"),				// ������
	_T("ONacAgent.exe"),				// ������ 
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
		// ������̿���
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		//ʵ������ʹ��Process32First��ȡ��һ�����յĽ���ǰ�����ĳ�ʼ������  
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
					// �������ˣ����ӿڴ򿪽��̣�������
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