 /// @file    fileutil.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-11 15:38:43
 
#include "fileutil.h"
#include <stdio.h>

AppendFile::AppendFile(std::string filename)
: fp_(fopen(filename.c_str(), "ae"))
{
	//此函数功能是将用户提供的缓冲区代替fp_本来的缓冲区。
	setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
	flush();
	fclose(fp_);
}

void AppendFile::append(const char *logline, const size_t len)
{
	size_t n = this->write(logline, len);	
	size_t remain = len - n;
	while(remain > 0){
		n = this->write(logline, remain);
		if(n == 0){
			int e = ferror(fp_);
			if(e){
				fprintf(stderr, "AppendFile::append() failed\n");
			}
			break;
		}
		remain -= n;
	}
	writtenBytes_ += len;
}

void AppendFile::flush()
{
	fflush(fp_);
}

size_t AppendFile::write(const char *logline, const size_t len)
{
	//此函数与fwrite的行为一致，除了它不使用锁之外，fwrite为了线程安全在函数中使用了锁。因此这个函数也是非线程安全的。
	return fwrite_unlocked(logline, 1, len, fp_);
}
