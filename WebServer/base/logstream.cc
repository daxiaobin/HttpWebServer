/// @file    logstream.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-14 09:02:02
 
#include "logstream.h"
#include <algorithm>

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

template<typename T>
size_t convert(char buf[], T value) //将数字转化为字符串
{
	T v = value;
	char *p = buf;
	do{
		int lsd = static_cast<int>(v % 10);
		v /= 10;
		*p++ = zero[lsd];
	}while(v != 0);

	if(value < 0){
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);
	return p - buf;
}

template<typename T>
void LogStream::fomatInteger(T v)
{
	if(buffer_.avail() > kMaxNumericSize){
		size_t len = convert(buffer_.current(), v);
		buffer_.add(len);
	}
}

LogStream& LogStream::operator<<(bool v){
	buffer_.append(v ? "1" : "0", 1);
	return *this;
}

LogStream& LogStream::operator<<(short v){
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v){
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v){
	fomatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v){
	fomatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v){
	fomatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v){
	fomatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v){
	fomatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v){
	fomatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(float v){
	*this << static_cast<double>(v);
	return *this;
}

LogStream& LogStream::operator<<(double v){
	if(buffer_.avail() > kMaxNumericSize){
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
		buffer_.add(len);
	}
	return *this;
}

LogStream& LogStream::operator<<(long double v){
	if(buffer_.avail() > kMaxNumericSize){
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
		buffer_.add(len);
	}
	return *this;
}

LogStream& LogStream::operator<<(char v){
	buffer_.append(&v, 1);
	return *this;
}

LogStream& LogStream::operator<<(const char *v){
	if(v){
		buffer_.append(v, strlen(v));
	}
	else{
		buffer_.append("(null)", 6);
	}
	return *this;
}

LogStream& LogStream::operator<<(const unsigned char *v){
	return operator<<(reinterpret_cast<const char *>(v));
}

LogStream& LogStream::operator<<(const std::string &v){
	buffer_.append(v.c_str(), v.size());
	return *this;
}
