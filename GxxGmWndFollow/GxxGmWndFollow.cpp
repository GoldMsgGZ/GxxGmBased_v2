// GxxGmWndFollow.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>


// 自定义消息处理函数的实现
LRESULT CALLBACK WindowProc(
							_In_  HWND hwnd,
							_In_  UINT uMsg,
							_In_  WPARAM wParam,
							_In_  LPARAM lParam
							)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

	case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)//按Esc键退出
			{
				PostQuitMessage(0);
				return 0;
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateWnd(HWND parent_hwnd, int x, int y, int width, int height)
{
	HINSTANCE hInstance;
	hInstance = GetModuleHandle(NULL);	//获取一个应用程序或动态链接库的模块句柄
	WNDCLASS Render_WND;
	Render_WND.cbClsExtra = 0;
	Render_WND.cbWndExtra = 0;
	Render_WND.hCursor = LoadCursor(hInstance, IDC_ARROW);	//鼠标风格
	Render_WND.hIcon = LoadIcon(hInstance, IDI_APPLICATION);	//图标风格
	Render_WND.lpszMenuName = NULL;	//菜单名
	Render_WND.style = CS_HREDRAW | CS_VREDRAW;	//窗口的风格
	Render_WND.hbrBackground = (HBRUSH)COLOR_WINDOW;	//背景色
	Render_WND.lpfnWndProc = WindowProc;	//【关键】采用自定义消息处理函数，也可以用默认的DefWindowProc
	Render_WND.lpszClassName = _T("RenderWindow");	//【关键】该窗口类的名称
	Render_WND.hInstance = hInstance;	//【关键】表示创建该窗口的程序的运行实体代号

	RegisterClass(&Render_WND);

	HWND hwnd = CreateWindow(
		_T("RenderWindow"),           //【关键】上面注册的类名lpszClassName，要完全一致  
		_T("Zombie"),  //窗口标题文字  
		WS_OVERLAPPEDWINDOW/*WS_CHILDWINDOW*/, //窗口外观样式  
		x,             //窗口相对于父级的X坐标  
		y,             //窗口相对于父级的Y坐标  
		width,              //窗口的宽度  
		height,             //窗口的高度  
		parent_hwnd,        //父窗口，还是必要的，估计可以跟随父窗口移动
		NULL,               //没有菜单，为NULL  
		hInstance,          //当前应用程序的实例句柄  
		NULL);              //没有附加数据，为NULL  

	//去标题栏
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION);

	//去边框
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME));

	// 显示窗口  
	ShowWindow(hwnd, SW_SHOW);

}



BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	RECT window_rect;
	GetWindowRect(hwnd, &window_rect);

	// chrome 子窗口标题：Chrome Legacy Window
	// 模拟的控件相对于子窗口坐标为(8,8) (208, 208)

	// 创建一个窗口
	int x = window_rect.left + 8;
	int y = window_rect.top + 8;
	int width = 200;
	int height = 200;
	CreateWnd(hwnd, x, y, width, height);

	TCHAR temp1[256] = {0}, temp2[256] = {0};
	::GetWindowText(hwnd, temp1, 255);
	_stprintf(temp2, _T("hwnd:%x text: %s"), hwnd, temp1);
	MessageBox(NULL, temp2, _T("cwnd"), MB_OK);
	return true;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	TCHAR caption[200] = {0};
	memset(caption, 0, sizeof(caption));
	::GetWindowText(hwnd, caption, 200);

	if(_tcscmp(caption, _T("")))
		std::cout<<caption<<std::endl;

	if(_tcsicmp(caption, _T("Test Chrome Element Pos - Google Chrome")) == 0)
	{
		// 枚举子窗口
		::EnumChildWindows(hwnd, EnumChildProc, 0);

		RECT window_rect;
		RECT client_rect;
		GetWindowRect(hwnd, &window_rect);
		GetClientRect(hwnd, &client_rect);

		//// 创建一个窗口
		//int x = rect.left;
		//int y = rect.top;
		//int width = rect.right - rect.left;
		//int height = rect.bottom - rect.top;
		//CreateWnd(hwnd, x, y, width, height);

		POINT point;
		ClientToScreen(hwnd, &point);

		std::cout<<caption<<std::endl;
	}

	return TRUE;

}

int _tmain(int argc, _TCHAR* argv[])
{
	// 首先枚举当前系统所有窗口
	EnumWindows(EnumWindowsProc, NULL);

	system("pause");
	return 0;
}

