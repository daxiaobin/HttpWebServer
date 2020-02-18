 /// @file    inetaddress.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-29 18:21:28
 
#ifndef INETADDRESS_H__
#define INETADDRESS_H__


#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

class InetAddress{
public:
	explicit InetAddress(uint16_t port);

	InetAddress(const std::string &ip, uint16_t port);

	InetAddress(const struct sockaddr_in &addr)
	: addr_(addr)
	{}

	//用于打印出主机和端口号
	std::string toHostPort() const;

	const struct sockaddr_in& getSockAddr() const { return addr_; }
	void setSockAddr(const struct sockaddr_in &addr) { addr_ = addr; }
private:
	struct sockaddr_in addr_;
};


#endif
