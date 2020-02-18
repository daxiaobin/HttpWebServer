 /// @file    logstring.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-10 07:56:33
 
#ifndef LOGGING_H__
#define LOGGING_H__


#include "logstream.h"
#include "timestamp.h"

class Logger{
public:
	enum LogLevel{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS
	};

	//ctor and dtor
	Logger(const char *filename, int line);
	Logger(const char *filename, int line, LogLevel level);
	Logger(const char *filename, int line, LogLevel level, const char *func);
	Logger(const char *filename, int line, bool isAbort);
	~Logger();

	//member function
	LogStream& stream() { return impl_.stream_; }
	static LogLevel loglevel();
	static void setLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char *msg, int len);
	typedef void (*FlushFunc)();
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
private:
	//private class
	class Impl{
	public:
		typedef Logger::LogLevel LogLevel;

		Impl(LogLevel level, const char *filename, int line);
		void formatTime();
		void finish();

		//data members
		LogStream stream_;
		LogLevel level_;
		int line_;
		std::string basename_;
	};
private:
	Impl impl_;
};

#define LOG_TRACE\
	if(Logger::loglevel() <= Logger::TRACE)	 Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream() 
#define LOG_DEBUG\
	if(Logger::loglevel() <= Logger::DEBUG)  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream() 
#define LOG_INFO\
	if(Logger::loglevel() <= Logger::INFO)   Logger(__FILE__, __LINE__).stream() 
#define LOG_WARN\
	Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR\
	Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL\
	Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR\
	Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL\
	Logger(__FILE__, __LINE__, true).stream()


#endif
