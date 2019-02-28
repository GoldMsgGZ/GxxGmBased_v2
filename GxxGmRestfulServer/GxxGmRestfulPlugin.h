#ifndef _GxxGmRestfulPlugin_H_
#define _GxxGmRestfulPlugin_H_

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

class GxxGmRestfulPlugin
{
public:
	virtual int Initialize() = 0;
	virtual int Uninitialize() = 0;

	virtual int RequestHandler(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) = 0;
};
#endif
