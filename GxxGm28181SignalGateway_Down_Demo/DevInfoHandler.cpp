#include "DevInfoHandler.h"
#include "GB28181AgentSDK.h"

#include "GB28181DownImp.h"

SIP_REPSOND_CODE DevInfoHandler::Handler(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData)
{
	GB28181DownImp *gb28181 = (GB28181DownImp *)pUserData;

	//////////////////////////////////////////////////////////////////////////
	// 首先检查查询类型，根据查询类型将参数放到不同的处理函数中去
	switch (stuQuery->eType)
	{
	case EnumQueryType::eQUE_DEV_INFO:
		// 设备信息查询
		{
			// 发消息给总线，查询指定设备的设备信息
			// 将查询条件发送给消息总线，这里的查询条件就是区域/设备/系统的国标ID
			// 这里组织一下Json，然后通过消息通信接口发给总线
			char json_str[4096] = {0};
			sprintf(json_str, "{\"type\":\"devinfo\",\"gbcode\":\"%s\"}", stuQuery->czGBCode);
			
		}
		break;
	case EnumQueryType::eQUE_DEV_STATUS:
		// 设备状态查询
		break;
	case EnumQueryType::eQUE_DEV_CATALOG:
		// 设备目录查询
		{
			StruDeviceCatalog stuCatlog;
			gs_strncpy(stuCatlog.czGBCode, stuQuery->czGBCode, STR_GBCODE_LEN);

			// 发消息给总线，查询组织架构和设备列表
			// 这里可能需要注意，如果内域集成数据太多，可能需要在前期做一些准备
			// 例如缓存组织架构列表，或者动态更新组织架构列表
			stuCatlog.ptrCatalog = gb28181->device_catlog_mgr_->ToGB28181Format(stuCatlog.iSumNum);

			int errCode = GB28181Agent_RespondDevCatalog(hSession, &stuCatlog, 0, false);
			if (errCode == GS28181_ERR_SUCCESS)
			{
				// 发送成功
			}
			else
			{
				// 发送失败
			}

			gb28181->device_catlog_mgr_->ReleaseInfoBuffer(&stuCatlog.ptrCatalog);
		}
		break;
	case EnumQueryType::eQUE_DEV_RECORDINDEX:
		// 设备录像索引查询
		break;
	case EnumQueryType::eQUE_DEV_CONFIGDOWNLOAD:
		// 设备配置查询
		break;
	case EnumQueryType::eQUE_DEV_PRESET:
		// 设备预置位查询
		break;
	case EnumQueryType::eQUE_DEV_UNKNOW:
		// 未知查询
		break;
	default:
		break;
	}

	return SIP_RESPONSE_CODE_SUCCESS;
}