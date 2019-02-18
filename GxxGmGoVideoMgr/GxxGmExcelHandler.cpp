#include "GxxGmExcelHandler.h"

GxxGmExcelHandler::GxxGmExcelHandler()
{

}

GxxGmExcelHandler::~GxxGmExcelHandler()
{

}

int GxxGmExcelHandler::Initialize()
{
	int errCode = 0;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);


	CLSID clsid;
	LPCOLESTR progID = L"Excel.Application";
	HRESULT hr = CLSIDFromProgID(progID, &clsid);
	if (FAILED(hr))
		return hr;

	// Start the server and get the IDispatch interface

	IDispatch *pXlApp = NULL;
	hr = CoCreateInstance(clsid  NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pXlApp));
	if (FAILED(hr))
		return hr;

	// Make Excel invisible. (i.e. Application.Visible = 0)
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"Visible", 1, x);
	}

	return errCode;
}

HRESULT GxxGmExcelHandler::AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...)
{
	// Begin variable-argument list
	va_list marker;
	va_start(marker, cArgs);

	if (!pDisp) 
	{
		_putws(L"NULL IDispatch passed to AutoWrap()");
		//_exit(0);
		return E_INVALIDARG;
	}

	// Variables used
	DISPPARAMS dp = { NULL, NULL, 0, 0 };
	DISPID dispidNamed = DISPID_PROPERTYPUT;
	DISPID dispID;
	HRESULT hr;

	// Get DISPID for name passed
	hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
	if (FAILED(hr))
	{
		wprintf(L"IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx\n", 
			ptName, hr);
		//_exit(0);
		return hr;
	}

	// Allocate memory for arguments
	VARIANT *pArgs = new VARIANT[cArgs + 1];
	// Extract arguments...
	for(int i=0; i < cArgs; i++) 
	{
		pArgs[i] = va_arg(marker, VARIANT);
	}

	// Build DISPPARAMS
	dp.cArgs = cArgs;
	dp.rgvarg = pArgs;

	// Handle special-case for property-puts
	if (autoType & DISPATCH_PROPERTYPUT)
	{
		dp.cNamedArgs = 1;
		dp.rgdispidNamedArgs = &dispidNamed;
	}

	// Make the call
	hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT,
		autoType, &dp, pvResult, NULL, NULL);
	if (FAILED(hr)) 
	{
		wprintf(L"IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx\n", 
			ptName, dispID, hr);
		//_exit(0);
		return hr;
	}

	// End variable-argument section
	va_end(marker);

	delete[] pArgs;

	return hr;
}