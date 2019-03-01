#include "GxxGmRestfulPluginDemo.h"
#include <sstream>

#include "Poco/StreamCopier.h"

// 这个宏必须在cpp文件里面实现，否则没有清单信息
POCO_BEGIN_MANIFEST(GxxGmAbstractPlugin)
POCO_EXPORT_CLASS(GxxGmRestfulPluginDemo)
POCO_END_MANIFEST

GxxGmRestfulPluginDemo::GxxGmRestfulPluginDemo()
{
	logger_ = NULL;
}

GxxGmRestfulPluginDemo::~GxxGmRestfulPluginDemo()
{
	logger_ = NULL;
}

int GxxGmRestfulPluginDemo::Initialize(GxxGmLoggerObject *logger)
{
	int errCode = 0;
	std::string errStr;

	logger_ = logger;
	WriteLog("Initialize");

	return errCode;
}

int GxxGmRestfulPluginDemo::Uninitialize()
{
	int errCode = 0;
	std::string errStr;

	WriteLog("Uninitialize");

	return errCode;
}

int GxxGmRestfulPluginDemo::RequestHandler(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	int errCode = 0;
	std::string errStr;

	try
	{
		// 这里会接收到外部发来的请求
		std::string uri = request.getURI();
		//std::string host = request.getHost();
		std::string method = request.getMethod();
		request.has("");

		// 读取发过来的数据
		// 读取数据
		std::istream &is = request.stream();

		std::ostringstream ostr;
		std::istream *ptr_rs = &is;
		Poco::StreamCopier::copyStream(*ptr_rs, ostr);

		std::string request_body = ostr.str();

		std::cout<<"收到请求："<<std::endl;
		std::cout<<method.c_str()<<" "/*<<host.c_str()*/<<uri.c_str()<<std::endl;

		if (request_body.size() > 0)
			std::cout<<request_body.c_str()<<std::endl;

		std::cout<<std::endl;

		// 响应信息
		response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		std::ostream &out = response.send();
	}
	catch(Poco::Exception &ex)
	{
		errCode = ex.code();
		errStr = ex.displayText();

		std::cout<<"接收请求时遇到异常，错误码："<<errCode<<"。错误信息："<<errStr.c_str()<<std::endl;
	}

	return errCode;
}

void GxxGmRestfulPluginDemo::WriteLog(std::string log, int log_level /* = LOG_LEVEL_INFO */)
{
	if (logger_)
		logger_->WriteLog(log, log_level);
}

void pocoInitializeLibrary()
{
	std::cout << "GxxGmRestfulPluginDemo initializing" << std::endl;
}

void pocoUninitializeLibrary()
{
	std::cout << "GxxGmRestfulPluginDemo uninitializing" << std::endl;
}