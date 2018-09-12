#include <iostream>
#include <string>
#include <Windows.h>
#include "..\GxxGmDSJSimulater\GxxGmDSJSimulater.h"

int main(int argc, const char *argv[])
{
	// 首先根据配置文件加载数据
	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string tmp = current_program_path;
	int pos = tmp.find_last_of('\\');

	std::string config_path = tmp.substr(0, pos + 1);
	config_path.append("GxxGmDSJSimulater.ini");

	// 读取配置参数
	char server_ip[4096] = {0};
	char server_port[4096] = {0};
	char server_gbcode[4096] = {0};
	char server_username[4096] = {0};
	char server_password[4096] = {0};

	DWORD bRet = 0;
	bRet = GetPrivateProfileStringA("[DeviceGateway]", "SERVER_IP", "127.0.0.1", server_ip, 4096, config_path.c_str());
	int errCode = GetLastError();
	bRet = GetPrivateProfileStringA("[DeviceGateway]", "SERVER_PORT", "5060", server_port, 4096, config_path.c_str());
	bRet = GetPrivateProfileStringA("[DeviceGateway]", "SERVER_GBCODE", "00000000000000000000", server_gbcode, 4096, config_path.c_str());
	bRet = GetPrivateProfileStringA("[DeviceGateway]", "SERVER_USERNAME", "1", server_username, 4096, config_path.c_str());
	bRet = GetPrivateProfileStringA("[DeviceGateway]", "SERVER_PASSWORD", "1", server_password, 4096, config_path.c_str());

	int simulater_count = 1;
	char local_ip[4096] = {0};
	int local_port_start = 0;
	char local_gbcode_pre[4096] = {0};
	int local_gbcode_id_start = 0;

	simulater_count = GetPrivateProfileIntA("[GxxGmDSJSimulater]", "[INSTANCE_COUNT]", 1, config_path.c_str());
	bRet = GetPrivateProfileStringA("[GxxGmDSJSimulater]", "[DEVICE_IP]", "127.0.0.1", local_ip, 4096, config_path.c_str());
	local_port_start = GetPrivateProfileIntA("[GxxGmDSJSimulater]", "[DEVICE_PORT_START]", 9000, config_path.c_str());
	bRet = GetPrivateProfileStringA("[GxxGmDSJSimulater]", "[GBCODE_PRE]", "00000000000000", local_gbcode_pre, 4096, config_path.c_str());
	local_gbcode_id_start = GetPrivateProfileIntA("[GxxGmDSJSimulater]", "[GBCODE_ID_START]", 0, config_path.c_str());

	// 根据设备数量启动所有设备
	for (int index = 0; index < simulater_count; ++index)
	{
		// 在这里计算模拟器端口号、国标码等信息
		//GxxGmDSJSimulater *simulater = new GxxGmDSJSimulater();
		//simulater->Initialize(local_ip)
	}


	return 0;
}