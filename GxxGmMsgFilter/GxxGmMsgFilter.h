#ifndef _GxxGmMsgFilter_H_
#define _GxxGmMsgFilter_H_

#ifndef DLLHOOK_API
#ifdef  GXXGMMSGFILTER_EXPORTS
#define DLLHOOK_API _declspec(dllexport)
#else
#define DLLHOOK_API _declspec(dllimport)
#endif
#endif

#include <Windows.h>
#include <tchar.h>

// 申请一段共享内存
#pragma data_seg("my_data")
HHOOK hook_callwndproc		= NULL;
HHOOK hook_callwndprocret	= NULL;
HHOOK hook_msgfilter		= NULL;
HHOOK hook_shell			= NULL;
HHOOK hook_sysmsgfilter		= NULL;
#pragma data_seg()
#pragma comment (linker,"/section:my_data,RWS")

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SysMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

// 这里需要定义一个钩子安装函数
DLLHOOK_API int InstallHooks(HWND hwnd);

#endif//_GxxGmMsgFilter_H_
