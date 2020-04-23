 /// @file    logging.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-10 10:17:12
 
#include "logging.h"
#include "thread.h"
#include "asynclogging.h"
#include <sys/time.h>

//用于将日志输出到文件中
static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static std::unique_ptr<AsyncLogging> asyncLoggingPtr_;
void once_init()
{
	asyncLoggingPtr_.reset(new AsyncLogging("LOG_FILE", 10 * 1024 * 1024));
	asyncLoggingPtr_->start();
}


const char *LogLevelName[Logger::NUM_LOG_LEVELS] = {
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

Logger::LogLevel initLogLevel()
{
	if(::getenv("LOG_TRACE")){
		return Logger::TRACE;
	}
	else if(::getenv("LOG_DEBUG")){
		return Logger::DEBUG;
	}
	else return Logger::INFO; //默认的日志输出级别是INFO
}

void defalutOutput(const char *msg, int len)
{
	//将日志输出到标准输出中
	//size_t n = fwrite(msg, 1, len, stdout);
	//if(n != static_cast<size_t>(len)){
	//}
	//将日志输出到文件中
	//pthread_once(&once_control, once_init);
	//asyncLoggingPtr_->append(msg, len);
}

void defalutFlush()
{
	fflush(stdout);
}


Logger::LogLevel	g_logLevel = initLogLevel();
Logger::OutputFunc  g_output   = defalutOutput;
Logger::FlushFunc   g_flush    = defalutFlush;

Logger::LogLevel Logger::loglevel()
{
	return g_logLevel;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
	g_logLevel = level;
}

void Logger::setOutput(Logger::OutputFunc func)
{
	g_output = func;
}

void Logger::setFlush(Logger::FlushFunc func)
{
	g_flush = func;
}

//日志的格式：日期 时间 线程id 级别 正文 - 源文件名:行号
Logger::Impl::Impl(LogLevel level, const char *filename, int line)
: stream_()
, level_(level)
, line_(line)
, basename_(filename)
{
	formatTime();
	stream_ << CurrentThread::tid();
	stream_ << ' ' << LogLevelName[level_] << " -- ";
}

void Logger::Impl::formatTime()
{
	struct timeval tv;
	time_t time;
	char str_t[26] = {0};
	gettimeofday(&tv, NULL);
	time = tv.tv_sec;
	struct tm *p_time = localtime(&time);
	strftime(str_t, 26, "%y-%m-%d %H:%M:%S ", p_time);
	stream_ << str_t;
}

void Logger::Impl::finish()
{
	stream_ << " -- " << basename_ << ":" << line_ << '\n';
}


Logger::Logger(const char *filename, int line)
: impl_(INFO, filename, line)
{}

Logger::Logger(const char *filename, int line, LogLevel level)
: impl_(level, filename, line)
{}

Logger::Logger(const char *filename, int line, LogLevel level, const char *func)
: impl_(level, filename, line)
{
	impl_.stream_ << func << ' ';
}

Logger::Logger(const char *filename, int line, bool isAbort)
: impl_(isAbort ? FATAL : ERROR, filename, line)
{}

//在Logger对象析构的时候将LogStream对象中的buffer中的内容进行输出。
Logger::~Logger()
{
	impl_.finish();
	const LogStream::Buffer& buf(stream().buffer());
	g_output(buf.data(), buf.length());
	if(impl_.level_ == FATAL){
		g_flush();
		abort();
	}
}
