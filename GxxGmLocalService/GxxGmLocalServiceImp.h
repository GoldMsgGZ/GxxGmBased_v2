#ifndef _GxxGmLocalServiceImp_H_
#define _GxxGmLocalServiceImp_H_

#include "NTService.h"

class GxxGmLocalServiceImp : public CNTService
{
public:
	GxxGmLocalServiceImp(LPCTSTR ServiceName, LPCTSTR DisplayName = 0);
	~GxxGmLocalServiceImp();

public:

};

#endif//_GxxGmLocalServiceImp_H_
