#ifndef _GxxGmPoliceStatuAndCasePlugin_H_
#define _GxxGmPoliceStatuAndCasePlugin_H_

#include "AbstractPlugin.h"
#include "Poco/ClassLibrary.h"
#include "Poco/Thread.h"

class GxxGmPoliceStatuAndCasePlugin : public AbstractPlugin
{
public:
	std::string name() const;

public:
	void start(Poco::Util::Application *app);

public:
	Poco::Thread working_thread_;
	static void WorkingThread(void* param);

public:
	Poco::Util::Application *app_;
};

#endif
