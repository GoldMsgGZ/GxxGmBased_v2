// GxxGmWinService.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <tchar.h>

#include "GxxGmService.h"

int _tmain(int argc, _TCHAR* argv[])
{
	GxxGmService service(_T("GxxGmFuckSophos"), _T("GxxGmFuckSophos"));
	service.RegisterService(argc, argv);

	return 0;
}

