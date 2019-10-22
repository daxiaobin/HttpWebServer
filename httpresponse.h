 /// @file    httpresponse.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-22 15:01:23
 
#ifndef HTTPRESPONSE_H__
#define HTTPRESPONSE_H__


#include <string>
#include <map>
using std::string;

enum HttpStatusCode{
	kUnknowCode,
	k200OK = 200,
	k301MovedPermanently = 301,
	k400BadRequest = 400,
	k404NotFound = 404
};

class Buffer;
class HttpResponse{
public:
	explicit HttpResponse(bool close)
	: statusCode_(kUnknowCode)
	, isShortConnection_(close)
	{}

	void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
	void setReasonMessage(const string &message) { reasonMessage_ = message; }
	void setBody(const string &str) { body_ = str; }
	void setCloseConnection(bool close) { isShortConnection_ = close; }
	void addHeaderField(const string &key, const string &value) { headerFields_[key] = value; }
	bool isShortConnection() const { return isShortConnection_; }

	void appendToBuffer(Buffer *buf) const;
private:
	HttpStatusCode statusCode_;
	string reasonMessage_;
	std::map<string, string> headerFields_;
	string body_;
	bool isShortConnection_;
};


#endif
