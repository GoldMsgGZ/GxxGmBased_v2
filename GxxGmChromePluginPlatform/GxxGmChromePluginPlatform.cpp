// GxxGmChromePluginPlatform.cpp : 定义控制台应用程序的入口点。
//
//
// 
// 测试服务器地址：http://www.blue-zero.com/WebSocket/

#include "stdafx.h"


#include "Poco/DateTimeFormatter.h"
#include "Poco/FileChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Timestamp.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#include "Poco/Net/WebSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Thread.h"
#include "Poco/Buffer.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/StreamCopier.h"

#include "Poco/JSON/Parser.h"


//////////////////////////////////////////////////////////////////////////
//
// WebSocket请求处理
class GxxGmWebSocketRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	GxxGmWebSocketRequestHandler(Poco::Util::Application *app, std::size_t buffer_size = 4096)
		: buffer_size_(buffer_size)
		, app_(app)

	{
	}

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		try
		{
			Poco::Net::WebSocket websocket(request, response);
			Poco::Buffer<char> buffer(buffer_size_);

			int flags = 0;
			int n = 0;
			do
			{
				// 接收数据，放入消息处理队列
				n = websocket.receiveFrame(buffer.begin(), static_cast<int>(buffer.size()), flags);

				// 分析协议，分发请求
				Poco::JSON::Parser parse;
				Poco::Dynamic::Var val;



				// 发送数据
				websocket.sendFrame(buffer.begin(), n, flags);

			} while (n > 0 || (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);
		}
		catch(Poco::Net::WebSocketException &exc)
		{
			switch (exc.code())
			{
			case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
				response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
				// fallthrough
			case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
			case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
			case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
				response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
				response.setContentLength(0);
				response.send();
				break;
			}
		}
		catch(Poco::Exception &ex)
		{
			std::cout<<ex.name()<<" "<<ex.code()<<std::endl;
		}
	}

private:
	std::size_t buffer_size_;
	Poco::Util::Application *app_;
};

class GxxGmWebSocketRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	GxxGmWebSocketRequestHandlerFactory(std::size_t buffer_size, Poco::Util::Application *app)
		: buffer_size_(buffer_size)
		, app_(app)
	{}

public:
	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		return new GxxGmWebSocketRequestHandler(app_, buffer_size_);
	}

private:
	std::size_t buffer_size_;
	Poco::Util::Application *app_;
};

class GxxGmServerApp : public Poco::Util::ServerApplication
{
public:
	void initialize(Poco::Util::Application& self)
	{
		Poco::Util::Application::initialize(self);

		std::string current_working_dir = Poco::Path::current();
		Poco::Path config_path(current_working_dir);
		config_path.append("config.ini");
		this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

		int log_level = config().getInt("LOG_INFO.LEVEL");

		// 初始化日志
		std::string name = "log_";
		name.append(Poco::DateTimeFormatter::format(Poco::Timestamp(), "%Y%m%d%H%M%S"));
		name.append(".log");

		Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel);
		fileChannel->setProperty("path", name);
		fileChannel->setProperty("rotation", "50 M");
		fileChannel->setProperty("archive", "timestamp");

		// 每条日志的时间格式
		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());
		patternFormatter->setProperty("pattern","[%Y-%m-%d %H:%M:%S] %p %s(%l): %t");

		//初始化　Channel
		Poco::AutoPtr<Poco::Channel> channel = new Poco::FormattingChannel(patternFormatter,fileChannel);
		logger().setChannel(channel);
		logger().setLevel(log_level);
	}

protected:
	int main(const std::vector<std::string>& args)
	{
		int errCode = Poco::Util::Application::EXIT_OK;
		std::string errstr;

		try
		{
			// 拿IP、端口进行绑定
			std::string ipaddress = config().getString("SERVICE_CFG.IP");
			unsigned int port = config().getUInt("SERVICE_CFG.PORT");
			Poco::Net::SocketAddress socket_address(ipaddress, port);
			Poco::Net::ServerSocket server_socket(socket_address);

			// 启动服务
			const int msgSize = 1024 * 1024;

			//Poco::Net::ServerSocket ss(0);
			Poco::Net::HTTPServer server(new GxxGmWebSocketRequestHandlerFactory(msgSize, this), server_socket, new Poco::Net::HTTPServerParams);
			server.start();
			waitForTerminationRequest();
			server.stop();
		}
		catch (Poco::Exception e)
		{
			errCode = e.code(); 
			errstr = e.displayText();
		}

		return Poco::Util::Application::EXIT_OK;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	GxxGmServerApp app;
	return app.run(argc, argv);
}

