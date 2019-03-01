#ifndef _GxxGmAbstractPlugin_H_
#define _GxxGmAbstractPlugin_H_

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#include "GxxGmRestfulPlugin.h"

class GxxGmAbstractPlugin
{
public:
	GxxGmAbstractPlugin();
	virtual ~GxxGmAbstractPlugin();

public:
	virtual int Initialize(GxxGmLoggerObject *logger) = 0;
	virtual int Uninitialize() = 0;
	virtual int RequestHandler(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) = 0;

public:
	GxxGmLoggerObject *logger_;
};

#endif//_GxxGmAbstractPlugin_H_
