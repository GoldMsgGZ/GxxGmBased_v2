#ifndef _GxxGmExtendBusiness_H_
#define _GxxGmExtendBusiness_H_

#include "tinyxml2.h"

class GxxGmExtendBusiness
{
public:
	GxxGmExtendBusiness();
	~GxxGmExtendBusiness();

public:
	int HandleRecv_BindUser_Response(tinyxml2::XMLElement *root);
	int HandleRecv_ConfigUpdateReceipt_Response(tinyxml2::XMLElement *root);
	int HandleRecv_DeviceInfo_Response(tinyxml2::XMLElement *root);
	int HandleRecv_LocationInfo_Response(tinyxml2::XMLElement *root);
	int HandleRecv_DeviceException_Response(tinyxml2::XMLElement *root);
	int HandleRecv_RemoteBindUser_Response(tinyxml2::XMLElement *root);
	int HandleRecv_RemoteRecord_Response(tinyxml2::XMLElement *root);
	int HandleRecv_RemoteTakePhoto_Response(tinyxml2::XMLElement *root);
	int HandleRecv_RemoteDeviceLock_Response(tinyxml2::XMLElement *root);
	int HandleRecv_RemoteUpdateSetting_Response(tinyxml2::XMLElement *root);
	int HandleRecv_EmergencyInfo_Response(tinyxml2::XMLElement *root);
};

#endif//_GxxGmExtendBusiness_H_
