// GxxGmWndFollow.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>


// �Զ�����Ϣ��������ʵ��
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
			if (wParam == VK_ESCAPE)//��Esc���˳�
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
	hInstance = GetModuleHandle(NULL);	//��ȡһ��Ӧ�ó����̬���ӿ��ģ����
	WNDCLASS Render_WND;
	Render_WND.cbClsExtra = 0;
	Render_WND.cbWndExtra = 0;
	Render_WND.hCursor = LoadCursor(hInstance, IDC_ARROW);	//�����
	Render_WND.hIcon = LoadIcon(hInstance, IDI_APPLICATION);	//ͼ����
	Render_WND.lpszMenuName = NULL;	//�˵���
	Render_WND.style = CS_HREDRAW | CS_VREDRAW;	//���ڵķ��
	Render_WND.hbrBackground = (HBRUSH)COLOR_WINDOW;	//����ɫ
	Render_WND.lpfnWndProc = WindowProc;	//���ؼ��������Զ�����Ϣ��������Ҳ������Ĭ�ϵ�DefWindowProc
	Render_WND.lpszClassName = _T("RenderWindow");	//���ؼ����ô����������
	Render_WND.hInstance = hInstance;	//���ؼ�����ʾ�����ô��ڵĳ��������ʵ�����

	RegisterClass(&Render_WND);

	HWND hwnd = CreateWindow(
		_T("RenderWindow"),           //���ؼ�������ע�������lpszClassName��Ҫ��ȫһ��  
		_T("Zombie"),  //���ڱ�������  
		WS_OVERLAPPEDWINDOW/*WS_CHILDWINDOW*/, //���������ʽ  
		x,             //��������ڸ�����X����  
		y,             //��������ڸ�����Y����  
		width,              //���ڵĿ��  
		height,             //���ڵĸ߶�  
		parent_hwnd,        //�����ڣ����Ǳ�Ҫ�ģ����ƿ��Ը��游�����ƶ�
		NULL,               //û�в˵���ΪNULL  
		hInstance,          //��ǰӦ�ó����ʵ�����  
		NULL);              //û�и������ݣ�ΪNULL  

	//ȥ������
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION);

	//ȥ�߿�
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME));

	// ��ʾ����  
	ShowWindow(hwnd, SW_SHOW);

}



BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	RECT window_rect;
	GetWindowRect(hwnd, &window_rect);

	// chrome �Ӵ��ڱ��⣺Chrome Legacy Window
	// ģ��Ŀؼ�������Ӵ�������Ϊ(8,8) (208, 208)

	// ����һ������
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
		// ö���Ӵ���
		::EnumChildWindows(hwnd, EnumChildProc, 0);

		RECT window_rect;
		RECT client_rect;
		GetWindowRect(hwnd, &window_rect);
		GetClientRect(hwnd, &client_rect);

		//// ����һ������
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
	// ����ö�ٵ�ǰϵͳ���д���
	EnumWindows(EnumWindowsProc, NULL);

	system("pause");
	return 0;
}

