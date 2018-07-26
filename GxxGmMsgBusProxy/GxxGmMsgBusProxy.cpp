#include "GxxGmMsgBusProxy.h"

GxxGmMsgBusProxy::GxxGmMsgBusProxy(GxxGmMsgBusRecevicer *recevicer)
: recevicer_(recevicer)
, connection_(NULL)
{

}

GxxGmMsgBusProxy::~GxxGmMsgBusProxy()
{

}

int GxxGmMsgBusProxy::Open(const char *bus_ip, unsigned int bus_port, const char *send_queue, const char *recevice_queue, const char *username /* = NULL */, const char *password /* = NULL */)
{
	char url[4096] = {0};
	sprintf(url, "amqp:tcp:%s:%d", bus_ip, bus_port);
	
	connection_ = new qpid::messaging::Connection(url, "");
	if (connection_ == NULL)
		return -1;

	// 设置连接的用户名密码
	if ((username != NULL) && (password != NULL))
	{
		connection_->setOption("username", username);
		connection_->setOption("password", password);
	}

	// 连接到qpid
	try {
		connection_->open();

		// 创建会话
		session_ = connection_->createSession();

		// 创建指定的发送者消息队列
		qpid::messaging::Address send_address("amq.topic");
		send_address.setSubject(send_queue);

		// 创建发送者
		sender_ = session_.createSender(send_address);

		// 创建指定的接收者消息队列
		qpid::messaging::Address recevice_address("amq.topic");
	}
}