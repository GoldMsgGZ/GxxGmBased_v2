#ifndef _MsgBus_H_
#define _MsgBus_H_

class MsgBus
{
public:
	MsgBus();
	~MsgBus();

public:
	int Connect(const char *bus_ip, unsigned int bus_port, const char *bus_topic, const char *bus_queue);
	void Disconnect();

public:
	int SendMsg(const char *msg);
};

#endif//_MsgBus_H_
