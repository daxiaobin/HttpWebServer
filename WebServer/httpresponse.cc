 /// @file    httpresponse.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-22 15:24:45
 
#include "httpresponse.h"
#include "buffer.h"
#include "base/logging.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void HttpResponse::appendToBuffer(Buffer *output) const
{
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", statusCode_); //服务端只支持HTTP/1.1
	output->append(buf);
	output->append(reasonMessage_);
	output->append("\r\n");

	snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
	output->append(buf);
	if(isShortConnection_){
		output->append("Connection:close\r\n");
	}
	else{
		output->append("Connection:Keep-Alive\r\n");
	}
	for(const auto &mem : headerFields_){
		output->append(mem.first);
		output->append(":");
		output->append(mem.second);
		output->append("\r\n");
	}

	output->append("\r\n");
	output->append(body_);
}

/*void HttpResponse::setBody(const string &path)
{
	assert(path.size() != 0);
	LOG_INFO <<	path;
	FILE *fp = fopen(path.c_str(), "r");
	struct stat statbuf;
	int ret = stat(path.c_str(), &statbuf);
	if(ret < 0){
		LOG_FATAL << "stat error";
	}
	char *ptr = new char[statbuf.st_size];
	size_t n = fread(ptr, 1, statbuf.st_size, fp);
	if(n != static_cast<size_t>(statbuf.st_size)){
		LOG_SYSERR << "fread error";
	}
	body_.assign(ptr, ptr + statbuf.st_size);
	delete[] ptr;
}*/
