#include "GxxGmHttpServer.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////////
void GxxGmRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	int errCode = 0;
	std::string errstr;
	try
	{
		// 从请求中读取数据
		std::string request_method = request.getMethod();
		std::string request_uri =  request.getURI();

		printf("收到了请求：\n%s %s\n", request_method.c_str(), request_uri.c_str());

		// 如果是POST，需要接收具体内容
		if (request_method.compare("POST") == 0)
		{
			// 得到数据长度
			std::streamsize request_content_length = request.getContentLength();
			if (request_content_length == 0)
			{
				// 没有body
				response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
				response.setContentType("text/html");
				std::ostream &out = response.send();
				out<<"";

				return ;
			}

			// 得到数据类型
			std::string request_content_type =  request.getContentType();

			// 读取数据
			std::istream &is = request.stream();

			std::ostringstream ostr;
			std::istream *ptr_rs = &is;
			Poco::StreamCopier::copyStream(*ptr_rs, ostr);

			std::string request_body = ostr.str();
			if (request_body.empty())
			{
				// 没有body
				response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
				response.setContentType("text/html");
				std::ostream &out = response.send();
				out<<"";

				return ;
			}
		}
		else if (request_method.compare("GET") == 0)
		{
			// GET数据
		}
		else
		{
			// 其他请求，不支持
			response.setStatus(Poco::Net::HTTPResponse::HTTP_VERSION_NOT_SUPPORTED);
			response.send();
		}

		//response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		//response.setContentType("text/html");

		//// 发送响应头
		//std::ostream &out = response.send();

		//out<<"<h1>Hello World!</h1>";
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("接收请求遇到了异常，异常码：%d，异常信息：%s\n", errCode, errstr.c_str());
	}
	
}

//////////////////////////////////////////////////////////////////////////
Poco::Net::HTTPRequestHandler* GxxGmRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
	return new GxxGmRequestHandler();
}

//////////////////////////////////////////////////////////////////////////
GxxGmHttpServer::GxxGmHttpServer()
: instance_(NULL)
{
	//
}

GxxGmHttpServer::~GxxGmHttpServer()
{
	//
	instance_->stopAll();
	delete instance_;
	instance_ = NULL;
}

int GxxGmHttpServer::main(const std::vector<std::string>& args)
{
	// 从参数中获得服务监听端口
	instance_ = new Poco::Net::HTTPServer(new GxxGmRequestHandlerFactory, Poco::Net::ServerSocket(9900), new Poco::Net::HTTPServerParams);
	instance_->start();
	Poco::Util::ServerApplication::waitForTerminationRequest();
	return Poco::Util::Application::EXIT_OK;
}

void GxxGmHttpServer::Start()
{
	instance_->start();
}

void GxxGmHttpServer::Stop()
{
	instance_->stop();
}

