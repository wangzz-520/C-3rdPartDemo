
#include <iostream>
#pragma warning(disable:4996)
#define GLOG_USE_GLOG_EXPORT
#include <glog/logging.h>

int main(int argc, char* argv[])
{
	FLAGS_log_dir = "./log";
	google::SetLogFilenameExtension(".log");

	// 初始化 Google 日志
	google::InitGoogleLogging("test");

	// 设置日志级别为 WARNING
	FLAGS_minloglevel = 1;

	// 记录不同级别的日志
	LOG(INFO) << "This is an info message";
	LOG(WARNING) << "This is a warning message";
	LOG(ERROR) << "This is an error message";

	// 使用条件日志
	int count = 20;
	LOG_IF(INFO, count > 10) << "Count is greater than 10";

	// 使用 CHECK 宏
	CHECK_NE(count, 0) << "Count should not be zero";

	// 严重错误日志，程序将终止
	LOG(FATAL) << "This is a fatal error";

	// 清理 Google 日志
	google::ShutdownGoogleLogging();

	return 0;
}


