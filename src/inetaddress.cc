 /// @file    inetaddress.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-29 18:43:18
 
#include "inetaddress.h"
#include "base/logging.h"
#include <cstring>

InetAddress::InetAddress(uint16_t port)
{
	bzero(&addr_, sizeof(struct sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
	bzero(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	if(::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0){
		LOG_SYSERR << "inet_pton error";
	}
}

std::string InetAddress::toHostPort() const
{
	char host[22] = "INVALID";
	::inet_ntop(AF_INET, &addr_.sin_addr, host, sizeof(host));
	uint16_t port = ntohs(addr_.sin_port);

	std::string result(host);
	result += ":";
	result += std::to_string(port);
	return result;
}
