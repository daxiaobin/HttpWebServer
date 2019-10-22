 /// @file    httprequest.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-21 19:46:35
 
#ifndef HTTPREQUEST_H__
#define HTTPREQUEST_H__


#include "base/timestamp.h"
#include <string>
#include <map>
using std::string;

enum Version{
	kUnknowVersion, kHttp10, kHttp11
};

enum ParseError{
	kUnFindCRLF,
	kRequestLineError,
	kHeaderFieldError,
};

enum HttpRequestParseState{
	kExpectRequestLine,
	kExpectHeaderField,
	kGotAll
};

class Buffer;
class HttpRequest{
public:
	HttpRequest() 
	: version_(kUnknowVersion)
	{}

	bool setMethod(const char *start, const char *end);
	string getMethod() const { return method_; }
	void setPath(const char *start, const char *end);
	string getPath() const { return path_; }
	void setVersion(Version v);
	Version getVersion() const { return version_; }
	void setQuery(const char *start, const char *end);
	string getQuery() const { return query_; }

	void setReceiveTime(const Timestamp &receiveTime){
		receiveTime_ = receiveTime;
	}
	Timestamp getReceiveTime() const{
		return receiveTime_;
	}

	int addHeaderField(const char *start, const char *colon, const char *end);
	string getHeaderFieldValue(const string &key) const;
	const std::map<string, string>& getHeaderFields() const { return headerFields_; }

	int parseRequest(Buffer *buf, Timestamp receiveTime);
private:
	int parseRequestLine(const char *start, const char *end);
private:
	string method_;
	string path_;
	Version version_;
	string query_;
	Timestamp receiveTime_;
	std::map<string, string> headerFields_;
	string body_;
};


#endif
