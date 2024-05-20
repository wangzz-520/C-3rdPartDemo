#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/Environment.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Stopwatch.h"
#include <iostream>
#include <iomanip>


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>


using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class TimeRequestHandler : public HTTPRequestHandler
	/// Return a HTML document with the current date and time.
{
public:
	TimeRequestHandler(const std::string& format) :
		_format(format)
	{
	}

	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		Application& app = Application::instance();
		app.logger().information("Request from " + request.clientAddress().toString());

		Timestamp now;
		std::string dt(DateTimeFormatter::format(now, _format));

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/html");

		std::ostream& ostr = response.send();
		ostr << "<html><head><title>HTTPTimeServer powered by POCO C++ Libraries</title>";
		ostr << "<meta http-equiv=\"refresh\" content=\"1\"></head>";
		ostr << "<body><p style=\"text-align: center; font-size: 48px;\">";
		ostr << dt;
		ostr << "</p></body></html>";
	}

private:
	std::string _format;
};


class TimeRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
	TimeRequestHandlerFactory(const std::string& format) :
		_format(format)
	{
	}

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
		if (request.getURI() == "/")
			return new TimeRequestHandler(_format);
		else
			return 0;
	}

private:
	std::string _format;
};


class HTTPTimeServer : public Poco::Util::ServerApplication
{
public:
	HTTPTimeServer() : _helpRequested(false)
	{
	}

	~HTTPTimeServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false));
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("A web server that serves the current date and time.");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		if (_helpRequested)
		{
			displayHelp();
		}
		else
		{
			// get parameters from configuration file
			unsigned short port = (unsigned short)config().getInt("HTTPTimeServer.port", 9980);
			std::string format(config().getString("HTTPTimeServer.format", DateTimeFormat::SORTABLE_FORMAT));
			int maxQueued = config().getInt("HTTPTimeServer.maxQueued", 100);
			int maxThreads = config().getInt("HTTPTimeServer.maxThreads", 16);
			ThreadPool::defaultPool().addCapacity(maxThreads);

			HTTPServerParams* pParams = new HTTPServerParams;
			pParams->setMaxQueued(maxQueued);
			pParams->setMaxThreads(maxThreads);

			// set-up a server socket
			ServerSocket svs(port);
			// set-up a HTTPServer instance
			HTTPServer srv(new TimeRequestHandlerFactory(format), svs, pParams);
			// start the HTTPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the HTTPServer
			srv.stop();
		}
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};


int main(int argc, char** argv)
{
	HTTPTimeServer app;
	return app.run(argc, argv);
}





int main2(int argc, char** argv)
{
	/* 解析json & 从文件中解析json */
	std::string jsonString = R"({"name": "John", "age": 30, "city": "New York"})";

	// 创建 JSON 解析器
	Poco::JSON::Parser parser;
	Poco::Dynamic::Var result;

	try {
		// 解析 JSON 字符串
		result = parser.parse(jsonString);
	}
	catch (const Poco::Exception& ex) {
		std::cerr << "JSON parsing error: " << ex.displayText() << std::endl;
		return 1;
	}

	// 将解析结果转换为 Poco::JSON::Object 类型
	Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

	// 获取和操作 JSON 对象中的值
	std::string name = object->getValue<std::string>("name");
	int age = object->getValue<int>("age");
	std::string city = object->getValue<std::string>("city");

	// 打印结果
	std::cout << "Name: " << name << std::endl;
	std::cout << "Age: " << age << std::endl;
	std::cout << "City: " << city << std::endl;


	/* 生成json & 写入到json文件 */
	// 创建 JSON 对象
	Poco::JSON::Object jsonObject;

	// 添加键值对
	jsonObject.set("name", "John");
	jsonObject.set("age", 30);
	jsonObject.set("city", "New York");

	// 将 JSON 对象转换为字符串
	std::ostringstream oss;
	Poco::JSON::Stringifier::stringify(jsonObject, oss);

	std::string jsonString2 = oss.str();

	// 打印生成的 JSON 字符串
	std::cout << jsonString2 << std::endl;


	return 0;
}