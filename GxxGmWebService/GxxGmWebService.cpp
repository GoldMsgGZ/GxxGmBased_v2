// GxxGmWebService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

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


//////////////////////////////////////////////////////////////////////////
//
// WebSocket请求处理
class GxxGmWebSocketRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	GxxGmWebSocketRequestHandler(std::size_t buffer_size = 4096)
		: buffer_size_(buffer_size)
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
				// 接收数据
				n = websocket.receiveFrame(buffer.begin(), static_cast<int>(buffer.size()), flags);

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
	}

private:
	std::size_t buffer_size_;
};

class GxxGmWebSocketRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	GxxGmWebSocketRequestHandlerFactory(std::size_t buffer_size)
		: buffer_size_(buffer_size)
	{}

public:
	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		return new GxxGmWebSocketRequestHandler(buffer_size_);
	}

private:
	std::size_t buffer_size_;
};


class GxxGmServerApp : public Poco::Util::ServerApplication
{
protected:
	int main(const std::vector<std::string>& args)
	{
		int errCode = Poco::Util::Application::EXIT_OK;
		std::string errstr;

		try
		{
			// 首先获得当前工作目录
			std::string current_working_dir = Poco::Path::current();
			Poco::Path config_path(current_working_dir);
			config_path.append("config.ini");
			this->loadConfiguration(config_path.toString(Poco::Path::PATH_NATIVE));

			// 拿IP、端口进行绑定
			std::string ipaddress = config().getString("SERVICE_CFG.IP");
			unsigned int port = config().getUInt("SERVICE_CFG.PORT");
			Poco::Net::SocketAddress socket_address(ipaddress, port);
			Poco::Net::ServerSocket server_socket(socket_address);

			// 启动服务
			const int msgSize = 64000;

			//Poco::Net::ServerSocket ss(0);
			Poco::Net::HTTPServer server(new GxxGmWebSocketRequestHandlerFactory(msgSize), server_socket, new Poco::Net::HTTPServerParams);
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

