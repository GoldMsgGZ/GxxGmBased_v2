// GxxGmOCXCheck.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	
	// 1. 首先检查插件是否已经安装
	// 1.1. 检测 HKMC\\SOFTWARE\\MozillaPlugins\\@gosun.com/npGSVideoPlugin 下是否有Path字段，值为多少
	//      然后检查该路径是否存在该插件

	// 1.2. 检测所有OCX控件，看看是否存在我们指定的控件

	// 2. 检测Chrome位数
	// 2.1. 检查当前系统是32位还是64位
	// 2.2. 如果是64位的，则判断运行中的Chrome是32位还是64位


	// 3. 结果写入文本
	return 0;
}

