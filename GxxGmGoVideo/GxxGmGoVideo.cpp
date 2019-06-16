#include "GxxGmGoVideo.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/PrintHandler.h"
#include "Poco/Latin1Encoding.h"
#include "Poco/TextConverter.h"

#include "GxxGmHttpServer.h"



GxxGmGoVideo::GxxGmGoVideo()
: http_session_(NULL)
, http_server_(new GxxGmHttpServer())
{
	//
}

GxxGmGoVideo::~GxxGmGoVideo()
{
	//
	if (http_session_)
	{
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;
		delete session;
	}
	http_session_ = NULL;

	if (http_server_)
	{
		http_server_->Stop();
		delete http_server_;
	}
	http_server_ = NULL;
}

int GxxGmGoVideo::Initialize(int http_port /* = 9900 */)
{
	int errCode = 0;
	std::string errStr;
	// ����һ���̣߳���������HTTP SERVER
	try
	{
		if (!http_server_thread_.isRunning())
		{
			http_server_thread_.start(GxxGmGoVideo::HttpServerThread, this);
		}
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errStr = ex.displayText();
	}

	return 0;
}

void GxxGmGoVideo::Destroy()
{
	http_server_->Stop();
}

int GxxGmGoVideo::Login(const char *govideo_ip, unsigned short govideo_port, const char *sequence_id /* = "admin" */, const char *login_id /* = "admin" */, int login_type /* = 107 */, const char *username /* = "admin" */, const char *password /* = "admin" */)
{
	int errCode = 0;
	std::string errstr;

	try {
		host_ = govideo_ip;
		port_ = govideo_port;
		http_session_ = new Poco::Net::HTTPClientSession(host_, port_);
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/LoginRequest?SequenceID=%s&LoginID=%s&LoginType=%d&UserName=%s&Password=%s&LoginIP=10.10.2.102",
			sequence_id, login_id, login_type, username, password);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		Poco::Dynamic::Var token = jsonObject->get("Token");

		token_ = token.toString();
		errCode = atoi(result_code.toString().c_str());

		// ��¼�ɹ��ˣ����������߳�
		if (!hb_thread_.isRunning())
		{
			//hb_thread_.start(GxxGmGoVideo::HeartBeatThread, this);
		}
	}
	catch (Poco::Net::NetException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("��¼��GoVideoʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::Logout()
{
	int errCode = 0;
	std::string errstr;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/LogoutRequest?SequenceID=1&Token=%s&LoginIP=192.168.30.17",
			token_);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
	}
	catch (Poco::Net::NetException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("��GoVideoע����¼ʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return 0;
}

int GxxGmGoVideo::DataUpdate()
{
	int errCode = 0;
	std::string errstr;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Server/DataUpdateRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::GetDeviceGatewayList()
{
	int errCode = 0;
	std::string errstr;

	try {

		govideo_services_.clear();

		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/GetAllServerRequest?SequenceID=5&Token=%s&StartRow=1&RowNum=200",
			token_.c_str());
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		// ��ȡ���з�������
		Poco::Dynamic::Var total_count = jsonObject->get("ServNum");
		int service_total_count = atoi(total_count.toString().c_str());

		// ��ȡ�����б�
		if (service_total_count > 0)
		{
			Poco::JSON::Array::Ptr result_service_list = jsonObject->getArray("ServiceInfoList");
			Poco::JSON::Array::ConstIterator iter = result_service_list->begin();
			for (; iter != result_service_list->end(); ++iter)
			{
				Poco::JSON::Object::Ptr service_info_json_object = iter->extract<Poco::JSON::Object::Ptr>();

				Poco::Dynamic::Var service_id				= service_info_json_object->get("ServID");
				Poco::Dynamic::Var service_name				= service_info_json_object->get("ServName");
				Poco::Dynamic::Var service_type_id			= service_info_json_object->get("ServTypeID");
				Poco::Dynamic::Var service_descript			= service_info_json_object->get("ServDesc");
				Poco::Dynamic::Var service_ip				= service_info_json_object->get("ServIP");
				Poco::Dynamic::Var service_port				= service_info_json_object->get("ServPort");
				Poco::Dynamic::Var username					= service_info_json_object->get("UserName");
				Poco::Dynamic::Var password					= service_info_json_object->get("Password");
				Poco::Dynamic::Var is_dns					= service_info_json_object->get("ISDNS");
				Poco::Dynamic::Var gbcode					= service_info_json_object->get("GBCode");
				Poco::Dynamic::Var version					= service_info_json_object->get("Version");
				Poco::Dynamic::Var license_info				= service_info_json_object->get("LicenseInfo");

				// ������
				GOVIDEO_SERVICE_INFO service_info;
				service_info.service_id_		= service_id.toString();
				service_info.service_name_		= service_name.toString();
				service_info.service_type_id_	= service_type_id.toString();
				service_info.service_descript_	= service_descript.toString();
				service_info.service_ip_		= service_ip.toString();
				service_info.service_port_		= service_port.toString();
				service_info.username_			= username.toString();
				service_info.password_			= password.toString();
				service_info.is_dns_			= is_dns.toString();
				service_info.gbcode_			= gbcode.toString();
				service_info.version_			= version.toString();
				service_info.license_info_		= license_info.toString();

				govideo_services_.push_back(service_info);
			}
		}
		
	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::RegisterDevice(GOVIDEO_DEVICE_INFO &device_info)
{
	int errCode = 0;
	std::string errstr;

	device_info.device_id_ = 0;

	try {
		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"DeviceInfoList\":[\
						{\
							\"DeviceID\":%d,\
							\"DevIndex\":1,\
							\"DeviceName\":\"%s\",\
							\"ModelID\":%d,\
							\"CategoryID\":%d,\
							\"DevCode\":\"%s\",\
							\"DevConnectionInfo\":\"%s\",\
							\"DevVersion\":\"%s\",\
							\"DevUserName\":\"%s\",\
							\"DevPassword\":\"%s\",\
							\"DevExInfo\":\"%s\",\
							\"GB28181Code\":\"%s\",\
							\"DevNameAbbr\":\"%s\",\
							\"Version\":0,\
							\"Remark\":\"%s\",\
							\"DGWServerID\":%d\
						}\
					],\
					\"DeviceCount\":1\
				}\
			}",
			device_info.device_id_,
			device_info.device_name_.c_str(), device_info.model_id_, device_info.category_id_,
			device_info.device_code_.c_str(), device_info.device_ip_.c_str(),
			device_info.device_version_.c_str(), device_info.device_username_.c_str(),
			device_info.device_password_.c_str(), device_info.device_extra_info_.c_str(),
			device_info.gb28181_code_.c_str(), device_info.device_name_abbr_.c_str(),
			device_info.remark_.c_str(), device_info.dgw_server_id_);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/SetDeviceRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request) << http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		// ����ע��ɹ����жϣ�Ӧ�����ض��Ĵ���ʽ�������õ����ص��豸�ڲ�ID
		if (jsonObject->isNull("DeviceParam"))
			return -1;

		Poco::Dynamic::Var result_device_count = jsonObject->get("ParamCount");
		int device_count = atoi(result_device_count.toString().c_str());
		if (device_count == 0)
			return -2;

		Poco::JSON::Array::Ptr device_array = jsonObject->getArray("DeviceParam");
		Poco::JSON::Array::ConstIterator iter = device_array->begin();
		for (; iter != device_array->end(); ++iter)
		{
			Poco::JSON::Object::Ptr device_info_json_object = iter->extract<Poco::JSON::Object::Ptr>();

			Poco::Dynamic::Var device_id				= device_info_json_object->get("DevID");
			Poco::Dynamic::Var device_index				= device_info_json_object->get("DevIndex");

			// ����Ӧ��ֱ�ӽ�device_id����������Ĳ��������Լ�ʱ�����豸�б���Ϣ
		}

	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}
	
	return errCode;
}

int GxxGmGoVideo::ModifyDevice(GOVIDEO_DEVICE_INFO &device_info)
{
	int errCode = 0;
	std::string errstr;

	try {
		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"DeviceInfoList\":[\
						{\
							\"DeviceID\":%d,\
							\"DevIndex\":1,\
							\"DeviceName\":\"%s\",\
							\"ModelID\":%d,\
							\"CategoryID\":%d,\
							\"DevCode\":\"%s\",\
							\"DevConnectionInfo\":\"%s\",\
							\"DevVersion\":\"%s\",\
							\"DevUserName\":\"%s\",\
							\"DevPassword\":\"%s\",\
							\"DevExInfo\":\"%s\",\
							\"GB28181Code\":\"%s\",\
							\"DevNameAbbr\":\"%s\",\
							\"Version\":0,\
							\"Remark\":\"%s\",\
							\"DGWServerID\":%d\
						}\
					],\
					\"DeviceCount\":1\
				}\
			}",
			device_info.device_id_,
			device_info.device_name_.c_str(), device_info.model_id_, device_info.category_id_,
			device_info.device_code_.c_str(), device_info.device_ip_.c_str(),
			device_info.device_version_.c_str(), device_info.device_username_.c_str(),
			device_info.device_password_.c_str(), device_info.device_extra_info_.c_str(),
			device_info.gb28181_code_.c_str(), device_info.device_name_abbr_.c_str(),
			device_info.remark_.c_str(), device_info.dgw_server_id_);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/SetDeviceRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request) << http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		//// ����ע��ɹ����жϣ�Ӧ�����ض��Ĵ���ʽ�������õ����ص��豸�ڲ�ID
		//if (jsonObject->isNull("DeviceParam"))
		//	return -1;

		//Poco::Dynamic::Var result_device_count = jsonObject->get("ParamCount");
		//int device_count = atoi(result_code.toString().c_str());
		//if (device_count == 0)
		//	return -2;

		//Poco::JSON::Array::Ptr device_array = jsonObject->getArray("DeviceParam");
		//Poco::JSON::Array::ConstIterator iter = device_array->begin();
		//for (; iter != device_array->end(); ++iter)
		//{
		//	Poco::JSON::Object::Ptr device_info_json_object = iter->extract<Poco::JSON::Object::Ptr>();

		//	Poco::Dynamic::Var device_id				= device_info_json_object->get("DevID");
		//	Poco::Dynamic::Var device_index				= device_info_json_object->get("DevIndex");

		//	// ����Ӧ��ֱ�ӽ�device_id����������Ĳ��������Լ�ʱ�����豸�б���Ϣ
		//}

	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::UnregisterDevice(const char *device_id)
{
	int errCode = -1;
	std::string errstr;

	try {
		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"OperatorType\":1,\
					\"DeviceIDCount\":1,\
					\"DeviceID\":[%s]\
				}\
			}",
			device_id);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/DelDeviceRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request) << http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;
	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::RegisterService(GOVIDEO_SERVICE_INFO &service_info)
{
	int errCode = 0;
	std::string errstr;

	service_info.service_id_ = "0";

	try {
		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"ServID\":%s,\
					\"ServName\":\"%s\",\
					\"ServTypeID\":%s,\
					\"ServDesc\":\"%s\",\
					\"ServIP\":\"%s\",\
					\"ServPort\":%s,\
					\"UserName\":\"%s\",\
					\"Password\":\"%s\",\
					\"ISDNS\":%s,\
					\"GBCode\":\"%s\",\
					\"Version\":\"%s\",\
					\"LicenseInfo\":\"%s\"\
				}\
			}",
			service_info.service_id_.c_str(), service_info.service_name_.c_str(),
			service_info.service_type_id_.c_str(), service_info.service_descript_.c_str(),
			service_info.service_ip_.c_str(), service_info.service_port_.c_str(),
			service_info.username_.c_str(), service_info.password_.c_str(),
			service_info.is_dns_.c_str(), service_info.gbcode_.c_str(),
			service_info.version_.c_str(), service_info.license_info_.c_str());

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/SetServerRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request) << http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		// ����ע��ɹ����жϣ�Ӧ�����ض��Ĵ���ʽ�������õ����ص��豸�ڲ�ID
		if (jsonObject->isNull("ServID"))
			return -1;

		Poco::Dynamic::Var result_service_id = jsonObject->get("ServID");

	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::ModifyService(GOVIDEO_SERVICE_INFO &service_info)
{
	int errCode = 0;
	std::string errstr;

	try {
		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"ServID\":%s,\
					\"ServName\":\"%s\",\
					\"ServTypeID\":%s,\
					\"ServDesc\":\"%s\",\
					\"ServIP\":\"%s\",\
					\"ServPort\":%s,\
					\"UserName\":\"%s\",\
					\"Password\":\"%s\",\
					\"ISDNS\":%s,\
					\"GBCode\":\"%s\",\
					\"Version\":\"%s\",\
					\"LicenseInfo\":\"%s\"\
				}\
			}",
			service_info.service_id_.c_str(), service_info.service_name_.c_str(),
			service_info.service_type_id_.c_str(), service_info.service_descript_.c_str(),
			service_info.service_ip_.c_str(), service_info.service_port_.c_str(),
			service_info.username_.c_str(), service_info.password_.c_str(),
			service_info.is_dns_.c_str(), service_info.gbcode_.c_str(),
			service_info.version_.c_str(), service_info.license_info_.c_str());

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/SetServerRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request) << http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		// ����ע��ɹ����жϣ�Ӧ�����ض��Ĵ���ʽ�������õ����ص��豸�ڲ�ID
		if (jsonObject->isNull("ServID"))
			return -1;

		Poco::Dynamic::Var result_service_id = jsonObject->get("ServID");

	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::UnregisterService(const char *service_id)
{
	int errCode = 0;
	std::string errstr;

	try {
		// �������󣬲�ѯ��ǰ���з���
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"ServID\":%s\
				}\
			}",
			service_id);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Serviceconfig/DelServerRequest");
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request) << http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		// �жϷ�����������Ϣ
		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
		{
			errCode = status;
			return errCode;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();
		if (json_str.empty())
		{
			return -1;
		}

		// ���ַ���תΪUTF-8

		Poco::Latin1Encoding latin1;
		Poco::UTF8Encoding utf8;
		Poco::TextConverter converter(latin1, utf8);
		std::string strUtf8;
		converter.convert(json_str, strUtf8);
		json_str = strUtf8;

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;
	}
	catch (Poco::Exception ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::GetAllDevices()
{
	int errCode = 0;
	std::string errstr;

	devices_.clear();

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		int loop_index = 0;
		int start_row = -1;
		while (true)
		{
			start_row = 500 * loop_index + 1;

			// ����ֻ���˴�����0��ʼ��ѯ500�������豸��������ʱ��������������Ҫ��ѯ���εģ�ֱ�����صĽ����Ϊ0��ʱ������ֹͣ��ѯ
			char query_string[4096] = {0};
			sprintf_s(query_string, 4096,
				"/GoVideo/ServerConfig/http://GoVideo/Serviceconfig/GetAllDeviceRequest?SequenceID=cyfiddev111&StartRow=%d&DevCount=500&Token=%s&lag=0",
				start_row,
				token_.c_str());
			std::string uri = query_string;
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

			// �������ʱ�򱯾���
			session->sendRequest(request);

			Poco::Net::HTTPResponse response;
			std::istream &is = session->receiveResponse(response);

			// �жϷ�����������Ϣ
			Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
			if (Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK != status)
			{
				errCode = status;
				return errCode;
			}

			std::ostringstream ostr;
			Poco::StreamCopier::copyStream(is, ostr);

			std::string json_str = ostr.str();
			if (json_str.empty())
			{
				return -1;
			}

			// ���ַ���תΪUTF-8

			Poco::Latin1Encoding latin1;
			Poco::UTF8Encoding utf8;
			Poco::TextConverter converter(latin1, utf8);
			std::string strUtf8;
			converter.convert(json_str, strUtf8);
			json_str = strUtf8;

			// �������
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var json = parser.parse(json_str);
			Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

			Poco::Dynamic::Var message = jsonObject->get("Message");
			jsonObject = message.extract<Poco::JSON::Object::Ptr>();
			Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
			errCode = atoi(result_code.toString().c_str());

			Poco::Dynamic::Var result_total_count = jsonObject->get("DevTotal");
			Poco::Dynamic::Var result_current_count = jsonObject->get("DevCount");

			int total_count = atoi(result_total_count.toString().c_str());
			int current_count = atoi(result_current_count.toString().c_str());
			if (current_count > 0)
			{
				Poco::JSON::Array::Ptr result_device_list = jsonObject->getArray("DeviceInfoList");
				Poco::JSON::Array::ConstIterator iter = result_device_list->begin();
				for (; iter != result_device_list->end(); ++iter)
				{
					Poco::JSON::Object::Ptr device_info_json_object = iter->extract<Poco::JSON::Object::Ptr>();

					Poco::Dynamic::Var device_id				= device_info_json_object->get("DeviceID");
					Poco::Dynamic::Var device_index				= device_info_json_object->get("DevIndex");
					Poco::Dynamic::Var device_name				= device_info_json_object->get("DeviceName");
					Poco::Dynamic::Var model_id					= device_info_json_object->get("ModelID");
					Poco::Dynamic::Var category_id				= device_info_json_object->get("CategoryID");
					Poco::Dynamic::Var device_code				= device_info_json_object->get("DevCode");
					Poco::Dynamic::Var device_connection_info	= device_info_json_object->get("DevConnectionInfo");
					Poco::Dynamic::Var device_version			= device_info_json_object->get("DevVersion");
					Poco::Dynamic::Var device_username			= device_info_json_object->get("DevUserName");
					Poco::Dynamic::Var device_password			= device_info_json_object->get("DevPassword");
					Poco::Dynamic::Var device_extra_info		= device_info_json_object->get("DevExInfo");
					Poco::Dynamic::Var gb28181_code				= device_info_json_object->get("GB28181Code");
					Poco::Dynamic::Var device_name_abbr			= device_info_json_object->get("DevNameAbbr");
					Poco::Dynamic::Var version					= device_info_json_object->get("Version");
					Poco::Dynamic::Var remark					= device_info_json_object->get("Remark");
					Poco::Dynamic::Var dgw_server_id			= device_info_json_object->get("DGWServerID");

					GOVIDEO_DEVICE_INFO *device_info = new GOVIDEO_DEVICE_INFO;

					device_info->device_id_				= atoi(device_id.toString().c_str());
					device_info->device_index_			= atoi(device_index.toString().c_str());
					device_info->device_name_			= device_name.toString();
					device_info->model_id_				= atoi(model_id.toString().c_str());
					device_info->category_id_			= atoi(category_id.toString().c_str());
					device_info->device_code_			= device_code.toString();
					device_info->device_ip_	= device_connection_info.toString();
					device_info->device_version_			= device_version.toString();
					device_info->device_username_		= device_username.toString();
					device_info->device_password_		= device_password.toString();
					device_info->device_extra_info_		= device_extra_info.toString();
					device_info->gb28181_code_			= gb28181_code.toString();
					device_info->device_name_abbr_		= device_name_abbr.toString();
					device_info->version_				= atoi(version.toString().c_str());
					device_info->remark_					= remark.toString();
					device_info->dgw_server_id_			= atoi(dgw_server_id.toString().c_str());

					//devices_.insert(std::pair<unsigned int, GOVIDEO_DEVICE_INFO*>(device_info->device_id_, device_info));
					devices_.push_back(device_info);
				}
			}

			if (devices_.size() >= total_count)
				break;

			++loop_index;
		}
	}
	catch (Poco::Net::NetException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("��GoVideo��ȡ�����豸��Ϣʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::JSON::JSONException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("��GoVideo��ȡ�����豸��Ϣʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::GetDeviceStatus(const char *device_gb28181_code, GOVIDEO_DEVICE_STATUS &device_status)
{
	int errCode = 0;
	std::string errstr;

	// �������Ѿ���ѯ�����豸�б������ȡGoVideoע����豸id
	unsigned int govideo_device_id = 0;
	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = devices_.begin(); iter != devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *dev_info = *iter;
		if (dev_info->gb28181_code_.compare(device_gb28181_code) == 0)
		{
			govideo_device_id = dev_info->device_id_;
			device_status.gb28181_code_ = device_gb28181_code;
			break;
		}
	}

	if (iter == devices_.end())
	{
		// û�ҵ��豸
		return -1;
	}

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Device/GetDeviceStatusRequest?SequenceID=5&DeviceID=%d&Token=%s",
			govideo_device_id, token_.c_str());
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);

		session->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		device_status.device_id_ = govideo_device_id;

		Poco::Dynamic::Var dev_status = jsonObject->get("DevStatus");
		device_status.status_ = atoi(dev_status.toString().c_str());

		Poco::Dynamic::Var channel_count = jsonObject->get("ChannelNum");
		if (atoi(channel_count.toString().c_str()) > 0)
		{
			Poco::JSON::Array::Ptr channel_status_list = jsonObject->getArray("ChannelStatusList");
			Poco::JSON::Array::ConstIterator iter = channel_status_list->begin();
			for (; iter != channel_status_list->end(); ++iter)
			{
				Poco::JSON::Object::Ptr channel_status_json_object = iter->extract<Poco::JSON::Object::Ptr>();

				Poco::Dynamic::Var channel_index = channel_status_json_object->get("ChnIndex");
				Poco::Dynamic::Var channel_status = channel_status_json_object->get("Status");

				int chn_index = atoi(channel_index.toString().c_str());
				int chn_status = atoi(channel_status.toString().c_str());

				device_status.chennal_status_.insert(std::pair<int, unsigned int>(chn_index, chn_status));
			}
		}
		
	}
	catch (Poco::Net::NetException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("��GoVideoע����¼ʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return 0;
}

int GxxGmGoVideo::GetAllDeviceStatus()
{
	devices_status_.clear();

	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = devices_.begin(); iter != devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *dev_info = *iter;
		GOVIDEO_DEVICE_STATUS device_status_;
		GetDeviceStatus(dev_info->gb28181_code_.c_str(), device_status_);

		devices_status_.push_back(device_status_);
	}

	return 0;
}

int GxxGmGoVideo::SubscriptionMsg(int subscript_type, const char *http_server_ip, int http_server_port)
{
	int errCode = 0;
	std::string errstr;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\"Message\":{\"SubscribeType\":\"%d\",\"ConsumerAddress\":\"http://%s:%d/notify_receiver\",\"SubscribeTopic\":\"0\"}}",
			subscript_type, http_server_ip, http_server_port);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Device/SetSubscribeRequest"
			);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);
		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request)<< http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (status != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
		{
			return status;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;
	}
	catch (Poco::Net::NetException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("����GoVideo������Ϣʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::GetRealStream(unsigned int device_id, std::string &stream_url, enum enStreamAgentType stream_type /* = StreamAgentNone */)
{
	int errCode = 0;
	std::string errstr;

	try {
		Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)http_session_;

		char body[4096] = {0};
		sprintf_s(body, 4096,
			"{\
				\"Message\":{\
					\"DeviceID\":\"%d\",\
					\"ClientType\":\"107\",\
					\"ChannelNum\":1,\
					\"StreamType\":1,\
					\"DirectDevice\":1,\
					\"StreamAgentType\":%d,\
					\"IPAddr\":\"10.10.16.59\",\
					\"Encode\":{\
						\"Video\":{\
							\"Streamcodeid\": 17,\
							\"Width\": 704,\
							\"Height\": 576,\
							\"Rate\": 12.5,\
							\"Bitrate\": 1024\
						},\
						\"Audio\":{\
							\"Streamcodeid\": 770,\
							\"Sample\": 8000,\
							\"Bits\": 16,\
							\"Channel\": 1,\
							\"Bitrate\": 16\
						},\
						\"Standardstream\":1\
					}\
				}\
			}",
			device_id, stream_type);

		char query_string[4096] = {0};
		sprintf_s(query_string, 4096,
			"/GoVideo/Device/GetStreamRequest"
			);
		std::string uri = query_string;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri, Poco::Net::HTTPRequest::HTTP_1_1);
		request.add("Content-Type", "application/json; charset=utf-8");
		request.add("SequenceID", "10086");
		request.add("Token", token_.c_str());

		std::string http_body(body);
		request.setContentLength((int)http_body.length());

		session->sendRequest(request)<< http_body;

		Poco::Net::HTTPResponse response;
		std::istream &is = session->receiveResponse(response);

		Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
		if (status != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
		{
			return status;
		}

		std::ostringstream ostr;
		Poco::StreamCopier::copyStream(is, ostr);

		std::string json_str = ostr.str();

		// �������
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var json = parser.parse(json_str);
		Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

		Poco::Dynamic::Var message = jsonObject->get("Message");
		jsonObject = message.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
		errCode = atoi(result_code.toString().c_str());
		if (errCode != 0)
			return errCode;

		Poco::Dynamic::Var stream_uri = jsonObject->get("StreamURI");
		stream_url = stream_uri.toString();
	}
	catch (Poco::Net::NetException &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
		printf("����ʧ�ܣ�%s\n", errstr.c_str());
	}
	catch (Poco::Exception &ex)
	{
		errCode = ex.code();
		errstr = ex.displayText();
	}

	return errCode;
}

int GxxGmGoVideo::GetRealStreamByGBCode(const char *device_gb28181_code, std::string &stream_url)
{
	int errCode = 0;

	std::vector<GOVIDEO_DEVICE_STATUS>::iterator iter;
	for (iter = devices_status_.begin(); iter != devices_status_.end(); ++iter)
	{
		GOVIDEO_DEVICE_STATUS *dev_status = &(*iter);
		if (dev_status->gb28181_code_.compare(device_gb28181_code) == 0)
		{
			if (dev_status->status_ == 1)
			{
				// �豸������
				errCode = -1;
				break;
			}

			errCode = GetRealStream(dev_status->device_id_, stream_url);
			break;
		}
	}

	return errCode;
}


void GxxGmGoVideo::HttpServerThread(void* param)
{
	int errCode = 0;
	GxxGmGoVideo *govideo = (GxxGmGoVideo *)param;

	//govideo->http_server_ = new GxxGmHttpServer();

	std::vector<std::string> params;
	params.push_back("9900");
	govideo->http_server_->run(params);

	// ����ȴ�100���������������Http����Ӧ��������
	Sleep(100);
}

void GxxGmGoVideo::HeartBeatThread(void* param)
{
	int errCode = 0;
	std::string errstr;

	GxxGmGoVideo *govideo = (GxxGmGoVideo *)param;

	while (true)
	{
		// ���tokenΪ�գ��򲻷�������
		// ʵ��������ƺ��ǰ����ĵ�Ҫ������������Ҳ����65-ϵͳ��֧��
		if (govideo->token_.empty())
		{
			Sleep(100);
			continue;
		}

		try {
			Poco::Net::HTTPClientSession *session = (Poco::Net::HTTPClientSession *)govideo->http_session_;

			char query_string[4096] = {0};
			sprintf_s(query_string, 4096,
				"/GoVideo/Serviceconfig/GetAllServerRequest?SequenceID=5&Token=%s&StartRow=1&RowNum=100",
				govideo->token_.c_str());
			std::string uri = query_string;
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPRequest::HTTP_1_1);
			//request.add("Content-Type", "application/json; charset=gb2312");
			//request.add("SequenceID", "10086");
			//request.add("Token", govideo->token_.c_str());

			//request.setContentLength(0);

			session->sendRequest(request);

			Poco::Net::HTTPResponse response;
			std::istream &is = session->receiveResponse(response);

			Poco::Net::HTTPResponse::HTTPStatus status = response.getStatus();
			if (status != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
			{
				Sleep(5000);
				continue;
			}

			std::ostringstream ostr;
			Poco::StreamCopier::copyStream(is, ostr);

			std::string json_str = ostr.str();
			if (json_str.empty())
			{
				Sleep(5000);
				continue;
			}

			// �������
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var json = parser.parse(json_str);
			Poco::JSON::Object::Ptr jsonObject = json.extract<Poco::JSON::Object::Ptr>();

			Poco::Dynamic::Var message = jsonObject->get("Message");
			jsonObject = message.extract<Poco::JSON::Object::Ptr>();
			Poco::Dynamic::Var result_code = jsonObject->get("OperResult");
			errCode = atoi(result_code.toString().c_str());

			// 5�뷢һ������
			Sleep(5000);
		}
		catch (Poco::Net::NetException &ex)
		{
			errCode = ex.code();
			errstr = ex.displayText();
			printf("������������ʧ�ܣ�%s\n", errstr.c_str());
		}
		catch (Poco::Exception &ex)
		{
			errCode = ex.code();
			errstr = ex.displayText();
			printf("������������ʧ�ܣ�%s\n", errstr.c_str());
		}
	}

}