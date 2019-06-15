#ifndef _GxxGmService_H_
#define _GxxGmService_H_

#include "NTService.h"

class GxxGmService : public CNTService
{
public:
	GxxGmService(LPCTSTR ServiceName, LPCTSTR DisplayName);
	virtual ~GxxGmService();

public:
	virtual void Run(DWORD argc, LPTSTR * argv);
	virtual void Stop();
};

#endif//_GxxGmService_H_
