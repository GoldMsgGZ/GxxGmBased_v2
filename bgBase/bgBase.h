#ifndef _bgBase_H_
#define _bgBase_H_

#include <string>

class bgBase
{
public:
	static void DebugStringOutput(const char *format, ...);
	static void DebugStringOutput(const wchar_t *format, ...);

	static void SetupMiniDumpMonitor(const char *aMiniDumpSavePath);
	static void SetupMiniDumpMonitor(const wchar_t *aMiniDumpSavePath);

	static int Base64Encode(const unsigned char *data, int data_len, std::string &encode_data);
	static int Base64Decode(std::string encode_data, unsigned char *data, int &data_len);

	static std::string GenerateGUID();
};

#endif//_bgBase_H_
