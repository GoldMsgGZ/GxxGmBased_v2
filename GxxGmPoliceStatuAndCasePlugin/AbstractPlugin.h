// AbstractPlugin.h
//
// This is used both by the class library and by the application.
#ifndef AbstractPlugin_INCLUDED
#define AbstractPlugin_INCLUDED

#include <string>
#include "Poco/Util/Application.h"

class AbstractPlugin
{
public:
	AbstractPlugin();
	virtual ~AbstractPlugin();
	virtual std::string name() const = 0;
	
public:
	virtual void start(Poco::Util::Application *app) = 0;

};
#endif // AbstractPlugin.h