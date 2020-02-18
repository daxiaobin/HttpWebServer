 /// @file    logstream.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-09 14:11:48
 
#ifndef LOGSTREAM_H__
#define LOGSTREAM_H__


#include "noncopyable.h"
#include <assert.h>
#include <string.h>
#include <string>

//全局const常量可以被头文件重复包含
const int kSmallBuffer = 4000; 
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer : Noncopyable{
public:
	FixedBuffer()
	: cur_(data_)
	{}

	void append(const char *buf, size_t len)
	{
		if(avail() > static_cast<int>(len)){
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}
	size_t avail() const { return end() - cur_; }
	const char* data() const { return data_; }
	char* current() { return cur_; }
	int length() const { return cur_ - data_; }
	void add(size_t len) { cur_ += len; }
	void reset() { cur_ = data_; }
	void bzero() { memset(data_, 0, sizeof(data_)); }
private:
	const char* end() const { return data_ + sizeof(data_); }
private:
	char data_[SIZE];
	char *cur_;
};

class LogStream : Noncopyable{
public:
	typedef FixedBuffer<kSmallBuffer> Buffer;

	LogStream& operator<<(bool);
	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);
//	LogStream& operator<<(const void*);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(long double);
	LogStream& operator<<(char);
	LogStream& operator<<(const char*);
	LogStream& operator<<(const unsigned char*);
	LogStream& operator<<(const std::string&);

	void append(const char *data, size_t len) { buffer_.append(data, len); }
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }
private:
	template<typename T>
	void fomatInteger(T);
private:
	Buffer buffer_;
	static const int kMaxNumericSize = 32;
};


#endif
