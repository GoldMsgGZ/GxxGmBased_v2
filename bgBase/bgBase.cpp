#include "bgBase.h"
#include "base64.h"
#include <Windows.h>
#include <iostream>
#include <objbase.h>
#include <tchar.h>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

using namespace std;

#define DEBUG_OUTPUT_BUFFER_LENGTH	10240


//////////////////////////////////////////////////////////////////////////
//
// 调试信息输出

void bgBase::DebugStringOutput(const char *format, ...)
{
	va_list argvs;
	char *value = new char[DEBUG_OUTPUT_BUFFER_LENGTH];
	ZeroMemory(value, sizeof(char) * DEBUG_OUTPUT_BUFFER_LENGTH);
	va_start(argvs, format);
	int n = vsprintf_s(value, DEBUG_OUTPUT_BUFFER_LENGTH, format, argvs);
	OutputDebugStringA(value);
	printf(value);
	va_end(argvs);

	delete [] value;
}

void bgBase::DebugStringOutput(const wchar_t *format, ...)
{
	va_list argvs;
	wchar_t *value = new wchar_t[DEBUG_OUTPUT_BUFFER_LENGTH];
	ZeroMemory(value, sizeof(wchar_t) * DEBUG_OUTPUT_BUFFER_LENGTH);
	va_start(argvs, format);
	int n = vswprintf_s(value, DEBUG_OUTPUT_BUFFER_LENGTH, format, argvs);
	OutputDebugStringW(value);
	wprintf(value);
	va_end(argvs);

	delete [] value;
}

//////////////////////////////////////////////////////////////////////////
//
// 崩溃转储处理逻辑

TCHAR g_MiniDumpPath[MAX_PATH] = {0};

LONG CreateDumpFile(EXCEPTION_POINTERS *aException)  
{
	if (_tcslen(g_MiniDumpPath) <= 0)
		return -1;

	// 创建Dump文件
	HANDLE hDumpFile = CreateFile(g_MiniDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hDumpFile == INVALID_HANDLE_VALUE)
		return -2;

	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = aException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;  

	// 写入Dump文件内容
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);

	return 0;
}

void bgBase::SetupMiniDumpMonitor(const char *aMiniDumpSavePath)
{
	// 先设置保存路径
	if (strlen(aMiniDumpSavePath) <= 0)
		return ;

#ifdef UNICODE
	MultiByteToWideChar(CP_ACP, 0, aMiniDumpSavePath, -1, g_MiniDumpPath, MAX_PATH);
#else
	_tcscpy_s(g_MiniDumpPath, MAX_PATH, aMiniDumpSavePath);
#endif

	// 注册Dump回调
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CreateDumpFile);

	return ;
}

void bgBase::SetupMiniDumpMonitor(const wchar_t *aMiniDumpSavePath)
{
	// 先设置保存路径
	if (wcslen(aMiniDumpSavePath) <= 0)
		return ;

#ifdef UNICODE
	_tcscpy_s(g_MiniDumpPath, MAX_PATH, aMiniDumpSavePath);
#else
	WideCharToMultiByte(CP_ACP, 0, aMiniDumpSavePath, -1, g_MiniDumpPath, MAX_PATH, NULL, NULL);
#endif

	// 注册Dump回调
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CreateDumpFile);

	return ;
}


//////////////////////////////////////////////////////////////////////////
//
// Base64编解码处理程序

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

int bgBase::Base64Encode(const unsigned char *data, int data_len, std::string &encode_data)
{
	int encode_len = Base64encode_len(data_len);
	char *encode_ = new char[encode_len];
	memset(encode_, 0, encode_len);

	int errCode = Base64encode(encode_, (const char *)data, data_len);

	encode_data = encode_;
	delete [] encode_;
	encode_ = NULL;

	return 0;
}

int bgBase::Base64Decode(std::string encode_data, unsigned char *data, int &data_len)
{
	if (!data)
		return -1;

	int decode_len_ = Base64decode_len(encode_data.c_str());
	if (decode_len_ > data_len)
		return -2;

	int errCode = Base64decode((char *)data, encode_data.c_str());
	data_len = decode_len_;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// GUID

std::string bgBase::GenerateGUID()
{
	CoInitialize(NULL);

	static char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf)
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}

	CoUninitialize();

	return buf;
}