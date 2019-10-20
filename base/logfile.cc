 /// @file    logfile.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-12 07:41:14
 
#include "logfile.h"
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


std::string getHostname()
{
	char buf[256] = {0};
	if(gethostname(buf, sizeof(buf)) == 0){
		return buf;
	}
	else{
		return "unknownhost";
	}
}

LogFile::LogFile(const std::string &basename, off_t rollSize, int flushInterval, int checkEveryN)
: basename_(basename)
, rollSize_(rollSize)
, flushInterval_(flushInterval)
, checkEveryN_(checkEveryN)
, startOfPeriod_(0)
, lastRoll_(0)
, lastFlush_(0)
{
	rollFile();
}

void LogFile::append(const char *logline, size_t len)
{
	MutexLockGuard autoLock(mutex_);
	append_unlocked(logline, len);
}

void LogFile::flush()
{
	MutexLockGuard autoLock(mutex_);
	file_->flush();
}

void LogFile::append_unlocked(const char *logline, size_t len)
{
	file_->append(logline, len);
	if(file_->writtenBytes() > rollSize_){
		rollFile(); //回滚日志文件条件一，同时也会刷新缓冲区
	}
	else{
		time_t now = time(NULL);
		time_t thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_; //计算结果是自1970.01.01以来到当天1点所经历的秒数
		if(thisPeriod != startOfPeriod_){
			rollFile(); //回滚日志文件条件二，同时也会刷新缓冲区
		}
		else if(now - lastFlush_ > flushInterval_){
			lastFlush_ = now;
			file_->flush(); //刷新缓冲区
		}
	}
}

//回滚文件和刷新缓冲区：
//	回滚文件的条件有两个：1.写入缓冲区的字节数大于rollSize_  2.每开始新的一天
//	每次回滚文件时file_会指向新分配的AppendFile对象，原来的对象也就析构掉了，析构时会刷新它的缓冲区。
//	当这次写入缓冲区的时间与上次刷新的时间超过3秒(flushInterval_)就再次刷新缓冲区。刷新缓冲区也就是将缓冲区中的数据写入到日志文件中去。

bool LogFile::rollFile()
{
	time_t now = time(NULL);
	std::string filename = getLogFileName(basename_, &now);
	time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;
	if(now > lastRoll_){
		lastRoll_ = now;
		lastFlush_ = now; //因为回滚日志文件也会先将其缓冲区刷新。
		startOfPeriod_ = start;
		file_.reset(new AppendFile(filename));
		return true;
	}
	return false;
}

//完整的文件名为：文件名.日期.主机名.进程号.log
std::string LogFile::getLogFileName(const std::string &basename, time_t* now)
{
	std::string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[26] = {0};
	struct tm* p_time = localtime(now);
	strftime(timebuf, sizeof(timebuf), ".%y%m%d-%H:%M:%S.", p_time);
	filename += timebuf;

	filename += getHostname();

	pid_t pid = getpid();
	filename += std::to_string(pid);

	filename += ".log";

	return filename;
}

