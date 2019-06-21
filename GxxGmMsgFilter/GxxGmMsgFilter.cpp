#include "GxxGmMsgFilter.h"

int InstallHooks(HWND hwnd)
{
	HMODULE hModule = GetModuleHandle(_T("GxxGmMsgFilter.dll"));
	if (hModule == NULL)
	{
		OutputDebugString(_T("没有找到GxxGmMsgFilter.dll的句柄"));
		return -1;
	}

	// 这里注册一个全局消息钩子
	hook_callwndproc	= SetWindowsHookEx(WH_CALLWNDPROC,		CallWndProc,	hModule, NULL);
	int errCode = GetLastError();

	hook_callwndprocret	= SetWindowsHookEx(WH_CALLWNDPROCRET,	CallWndRetProc, hModule, NULL);
	errCode = GetLastError();

	hook_msgfilter		= SetWindowsHookEx(WH_MSGFILTER,		MessageProc,	hModule, NULL);
	errCode = GetLastError();

	hook_shell			= SetWindowsHookEx(WH_SHELL,			ShellProc,		hModule, NULL);
	errCode = GetLastError();

	hook_sysmsgfilter	= SetWindowsHookEx(WH_SYSMSGFILTER,		SysMsgProc,		hModule, NULL);
	errCode = GetLastError();

	return 0;
}


LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPSTRUCT msg = (PCWPSTRUCT)lParam;
	if (msg->message == WM_MOVE)
	{
		OutputDebugString(_T("窗口移动消息\n"));
	}

	// 向下传递消息
	return CallNextHookEx(hook_callwndproc, nCode, wParam, lParam);
}

LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPRETSTRUCT msg = (PCWPRETSTRUCT)lParam;
	if (msg->message == WM_MOVE)
	{
		OutputDebugString(_T("窗口移动消息\n"));
	}

	// 向下传递消息
	return CallNextHookEx(hook_callwndprocret, nCode, wParam, lParam);
}

LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		OutputDebugString(_T("窗口移动消息\n"));
	}

	// 向下传递消息
	return CallNextHookEx(hook_msgfilter, nCode, wParam, lParam);
}

LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		OutputDebugString(_T("窗口移动消息\n"));
	}

	// 向下传递消息
	return CallNextHookEx(hook_shell, nCode, wParam, lParam);
}

LRESULT CALLBACK SysMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		OutputDebugString(_T("窗口移动消息\n"));
	}

	switch (nCode)
	{
	case MSGF_DIALOGBOX:
		// 窗口消息
		break;
	case MSGF_MENU:
		break;
	case MSGF_SCROLLBAR:
		// 滚动条滚动消息
		break;
	}

	// 向下传递消息
	return CallNextHookEx(hook_sysmsgfilter, nCode, wParam, lParam);
}