
// GxxGmChromeWndFollow.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGxxGmChromeWndFollowApp:
// �йش����ʵ�֣������ GxxGmChromeWndFollow.cpp
//

class CGxxGmChromeWndFollowApp : public CWinAppEx
{
public:
	CGxxGmChromeWndFollowApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGxxGmChromeWndFollowApp theApp;