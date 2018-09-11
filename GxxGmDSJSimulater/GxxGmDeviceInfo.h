#ifndef _GxxGmDeviceInfo_H_
#define _GxxGmDeviceInfo_H_

#include <string>

// 设备基本信息
typedef struct _DEVICE_BASE_INFO_
{
	std::string carrieroperator_;	// 运营商
	std::string net_type_;			// 网络类型
	std::string signal_;			// 信号强度
	std::string battery_;			// 剩余电量
	std::string storage_;			// 剩余容量
	std::string cpu_;				// CPU占用率
	std::string version_;			// 系统版本号
	std::string local_record_;		// 本地录像状态
} DEVICE_BASE_INFO, *PDEVICE_BASE_INFO;

// 设备定位信息
typedef struct _DEVICE_LOCATION_INFO_
{
	std::string division_ns_;
	std::string division_ew_;
	std::string radius_;
	std::string longitude_;
	std::string latitude_;
	std::string direction_;
	std::string speed_;
	std::string height_;
	std::string location_time_;
	std::string satellites_;
	std::string location_available_;
} DEVICE_LOCATION_INFO, *PDEVICE_LOCATION_INFO;

// 设备异常信息
typedef struct _DEVICE_EXCEPTION_INFO_
{
	std::string storage_;
	std::string battery_;
	std::string ccd_;
	std::string mic_;
} DEVICE_EXCEPTION_INFO, *PDEVICE_EXCEPTION_INFO;

#endif//_GxxGmDeviceInfo_H_
