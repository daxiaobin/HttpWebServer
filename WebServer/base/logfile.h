 /// @file    logfile.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-11 16:57:17
 
#ifndef LOGFILE_H__
#define LOGFILE_H__


#include "noncopyable.h"
#include "mutexlock.h"
#include "fileutil.h"
#include <memory>

class LogFile : Noncopyable{
public:
	LogFile(const std::string &basename, off_t rollSize, int flushInterval = 3, int checkEveryN = 1024);

	void append(const char *logline, size_t len);
	void flush();
	bool rollFile();
private:
	void append_unlocked(const char *logline, size_t len);
	static std::string getLogFileName(const std::string &basename, time_t* now);
private:
	const std::string basename_;
	const off_t rollSize_;
	const int flushInterval_;
	const int checkEveryN_;

	int count_;
	time_t startOfPeriod_; //代表自1970.01.01以来到创建日志文件当天1点所经过的秒数
	time_t lastRoll_;
	time_t lastFlush_;

	MutexLock mutex_;
	std::unique_ptr<AppendFile> file_;

	static const int kRollPerSeconds_ = 24 * 60 * 60; //每过一天的秒数进行回滚日志文件
};


#endif
