#ifndef _GxxGmMsgBusProxy_H_
#define _GxxGmMsgBusProxy_H_

#include "qpid/messaging/Address.h"
#include "qpid/messaging/Connection.h"
#include "qpid/messaging/Message.h"
#include "qpid/messaging/Receiver.h"
#include "qpid/messaging/Sender.h"
#include "qpid/messaging/Session.h"


class GxxGmMsgBusRecevicer
{
public:
	virtual int Recv(const char *msg) = 0;
};

class GxxGmMsgBusProxy
{
public:
	GxxGmMsgBusProxy(GxxGmMsgBusRecevicer *recevicer);
	~GxxGmMsgBusProxy();

public:
	int Open(const char *bus_ip, unsigned int bus_port, const char *username = NULL, const char *password = NULL);
	void Close();

public:
	int SendMsg(const char *msg);

private:
	qpid::messaging::Connection *connection_;
};

#endif//_GxxGmMsgBusProxy_H_
