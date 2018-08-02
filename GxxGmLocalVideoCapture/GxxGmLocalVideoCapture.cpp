#include "GxxGmLocalVideoCapture.h"

GxxGmLocalVideoCapture::GxxGmLocalVideoCapture()
{

}

GxxGmLocalVideoCapture::~GxxGmLocalVideoCapture()
{

}

int GxxGmLocalVideoCapture::EnumVideoDevices()
{
	// 这里枚举所有本地摄像头
	int errCode = 0;

	TDeviceName name;   
	HRESULT hr;  

	// 初始化  
	video_devices.clear();  

	// 初始化COM  
	hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );  
	if (FAILED(hr))  
	{  
		return hr;  
	}  

	// 创建系统设备枚举器实例  
	ICreateDevEnum *pSysDevEnum = NULL;  
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);  
	if (FAILED(hr))  
	{  
		CoUninitialize();  
		return hr;  
	}  

	// 获取设备类枚举器  
	int index = 0;
	IEnumMoniker *pEnumCat = NULL;  
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);  
	if (hr == S_OK)   
	{  
		// 枚举设备名称  
		IMoniker *pMoniker = NULL;  
		ULONG cFetched;  
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)  
		{  
			IPropertyBag *pPropBag;  
			hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropBag);  
			if (SUCCEEDED(hr))  
			{  
				// 获取设备友好名  
				VARIANT varName;  
				VariantInit(&varName);  

				hr = pPropBag->Read(L"FriendlyName", &varName, NULL);  
				if (SUCCEEDED(hr))  
				{  
					StringCchCopy(name.FriendlyName, MAX_FRIENDLY_NAME_LENGTH, varName.bstrVal);  

					// 获取设备Moniker名  
					LPOLESTR pOleDisplayName = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(MAX_MONIKER_NAME_LENGTH * 2));  
					if (pOleDisplayName != NULL)  
					{  
						hr = pMoniker->GetDisplayName(NULL, NULL, &pOleDisplayName);  
						if (SUCCEEDED(hr))  
						{  
							StringCchCopy( name.MonikerName, MAX_MONIKER_NAME_LENGTH, pOleDisplayName );  
							video_devices.insert(std::pair<int, TDeviceName>(index, name));
							//vectorDevices.push_back( name );  
							++index;
						}  

						CoTaskMemFree( pOleDisplayName );  
					}  
				}  

				VariantClear( &varName );  
				pPropBag->Release();                       
			}  

			pMoniker->Release();  
		} // End for While  

		pEnumCat->Release();  
	}  

	pSysDevEnum->Release();  
	CoUninitialize();

	return errCode;
}

int GxxGmLocalVideoCapture::StartCapture(int video_index)
{
	// 这里开始进行视频采集，我们应该会收到摄像头返回的编码包
	// 需要一个回调接口将编码包传出去
	int errCode = 0;

	return errCode;
}

int GxxGmLocalVideoCapture::StopCapture()
{
	return 0;
}