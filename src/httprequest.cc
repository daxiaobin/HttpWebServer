 /// @file    httprequest.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-22 08:31:03
 
#include "httprequest.h"
#include "buffer.h"

bool HttpRequest::setMethod(const char *start, const char *end)
{
	string method(start, end);
	if(method != "GET" && method != "HEAD" && method != "POST" && method != "PUT"){
		method.assign("invaild");
	}
	method_ = method;
	return method_ != "invaild";
}

void HttpRequest::setPath(const char *start, const char *end)
{
	path_.assign(start, end);
}

void HttpRequest::setVersion(Version v)
{
	version_ = v;
}

void HttpRequest::setQuery(const char *start, const char *end)
{
	query_.assign(start, end);
}

int HttpRequest::addHeaderField(const char *start, const char *colon, const char *end)
{
	if(isspace(*(colon - 1))){ //冒号前不允许有空格
		return kHeaderFieldError;
	}
	string key(start, colon);
	++colon;
	while(colon < end && isspace(*colon)){ //去掉value前面的空格
		++colon;
	}
	string value(colon, end);
	while(!value.empty() && isspace(value[value.size() - 1])){ //去掉value后面的空格
		value.resize(value.size() - 1);
	}
	headerFields_[key] = value;
	return kExpectHeaderField;
}

string HttpRequest::getHeaderFieldValue(const string &key) const
{
	string value;
	std::map<string, string>::const_iterator it = headerFields_.find(key);
	if(it != headerFields_.end()){
		value = it->second;
	}
	return value;
}

int HttpRequest::parseRequest(Buffer *buf, Timestamp receiveTime)
{
	setReceiveTime(receiveTime);
	const char *crlf = buf->findCRLF();
	if(crlf){
		int state = parseRequestLine(buf->peek(), crlf);
		if(state == kExpectRequestLine){ //解析请求行
			buf->retrieveUntil(crlf + 2);
			while((crlf = buf->findCRLF()) != nullptr){ //解析头部字段
				const char *colon = std::find(buf->peek(), crlf, ':');	
				if(colon != crlf){
					state = addHeaderField(buf->peek(), colon, crlf); //添加头部字段key-value到map中
					buf->retrieveUntil(crlf + 2);
					if(state == kHeaderFieldError) 
						return state;
				}
				else{ //遇到空行，解析头部完成，保存实体数据
					buf->retrieveUntil(crlf + 2);
					body_ = buf->retrieveAsString();
				}
			}
		}
		else{
			return state;
		}
	}
	else{
		return kUnFindCRLF;
	}

	return kGotAll;
}

int HttpRequest::parseRequestLine(const char *start, const char *end)
{
	const char *space = std::find(start, end, ' ');
	if(space == end || !setMethod(start, space)) return kRequestLineError;
	start = space + 1;
	space = std::find(start, end, ' ');
	if(space != end){
		const char *question = std::find(start, space, '?');
		if(question != space){
			setPath(start, question);
			setQuery(question + 1, space);
		}
		else{
			setPath(start, space);
		}
		start = space + 1;
		if(end - start == 8 && std::equal(start, end - 1, "HTTP/1.")){
			if(*(end - 1) == '1') setVersion(kHttp11);
			if(*(end - 1) == '0') setVersion(kHttp10);
		}
		else {
			return kRequestLineError;
		}
		return kExpectRequestLine;
	}
	return kRequestLineError;
}
