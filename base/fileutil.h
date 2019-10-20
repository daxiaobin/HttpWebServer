 /// @file    fileutil.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-11 15:29:45
 
#ifndef FILEUTIL_H__
#define FILEUTIL_H__


#include "noncopyable.h"
#include <string>

class AppendFile : Noncopyable{
public:
	explicit AppendFile(std::string filename);
	~AppendFile();

	void append(const char *logline, const size_t len);
	void flush();
	off_t writtenBytes() const { return writtenBytes_; }
private:
	size_t write(const char *logline, const size_t len);
private:
	FILE *fp_;
	off_t writtenBytes_;
	char buffer_[64*1024];
};


#endif
