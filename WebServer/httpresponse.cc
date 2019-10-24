 /// @file    httpresponse.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-22 15:24:45
 
#include "httpresponse.h"
#include "buffer.h"
#include <stdio.h>

void HttpResponse::appendToBuffer(Buffer *output) const
{
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", statusCode_); //服务端只支持HTTP/1.1
	output->append(buf);
	output->append(reasonMessage_);
	output->append("\r\n");

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
