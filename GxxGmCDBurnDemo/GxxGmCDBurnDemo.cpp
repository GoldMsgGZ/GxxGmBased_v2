// GxxGmCDBurnDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <objbase.h>

#import "E:\\Project\\Company\\刻录\\ocx\\CDRecordOcx\\test\\CDRecordOcxD.ocx" no_namespace


int _tmain(int argc, _TCHAR* argv[])
{
	typedef HRESULT (__stdcall * pfnHello)(REFCLSID,REFIID,void**);
	pfnHello fnHello= NULL;

	HINSTANCE hdllInst = LoadLibrary("E:\\Project\\Company\\刻录\\ocx\\CDRecordOcx\\test\\CDRecordOcxD.ocx");
	fnHello=(pfnHello)GetProcAddress(hdllInst,"DllGetClassObject");

	if (fnHello != 0)
	{
		IClassFactory* pcf = NULL;
		HRESULT hr=(fnHello)(CLSID_GetRes,IID_IClassFactory,(void**)&pcf);
		if (SUCCEEDED(hr) && (pcf != NULL))
		{
			IGetRes* pGetRes = NULL;
			hr = pcf->CreateInstance(NULL, IID_IFoo, (void**)&pGetRes);
			if (SUCCEEDED(hr)   && (pFoo != NULL))
			{
				pGetRes->Hello();
				pGetRes->Release();
			}
			pcf->Release();
		}
	} 
	FreeLibrary(hdllInst);

	//CoInitialize(NULL);

	//CLSID clsid;
	//HRESULT hret = CLSIDFromProgID(OLESTR("CDRecordOcxLib._DCDRecordOcx"), &clsid);

	//CoUninitialize();
	return 0;
}

