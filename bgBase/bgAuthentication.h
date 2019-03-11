#ifndef _bgAuthentication_H_
#define _bgAuthentication_H_

#include <string>

class bgAuthentication
{
public:
	bgAuthentication();
	~bgAuthentication();

public:
	// 检查本机保存的授权信息，是否在授权范围内
	// 返回结果
	bool CheckAuthentication();

	// 生成新的授权申请码
	std::string GetNewAuthenRequestCode();

	// 验证管理员发回的授权码
	bool VerifyAuthenticateResponseCode(std::string response_code);

private:
	// 抓取收拾服务器的时间
	__int64 GetCurrentFromNTPServer();

	// 获取机器码
	std::string GetMachineCode();

	// 读取本地保存的授权信息
	std::string GetAuthenticateInfo();
	
	// 将验证过的授权信息写入本地
	bool SetAuthenticateInfo(std::string authen_code);

private:
	std::string GetProcessorIdentifier();
	std::string GetProcessorProcessorNameString();
	std::string GetProcessorVendorIdentifier();

	std::string GetBIOSVendor();
	std::string GetBIOSVersion();

	std::string GetDiskIdentifier();
};

// 声明一个单例对象
extern bgAuthentication authenticater;

#endif//_bgAuthentication_H_
