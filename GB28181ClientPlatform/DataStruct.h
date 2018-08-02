#ifndef _DataStruct_H_ 
#define _DataStruct_H_

#include <string>
#include <vector>

struct ChnInfo
{
	std::string name;
	std::string gbcode;
	std::string father_gbcode;
};

struct DevInfo
{
	std::string name;
	std::string gbcode;
	std::string father_gbcode;
	int type;

	std::string latitude;
	std::string longtitude;

	std::vector<ChnInfo> sub_chns;
};

struct OrgInfo 
{
	std::string name;
	std::string gbcode;
	std::string father_gbcode;

	std::vector<OrgInfo> sub_orgs;
	std::vector<DevInfo> sub_devs;
};

#endif//_DataStruct_H_
