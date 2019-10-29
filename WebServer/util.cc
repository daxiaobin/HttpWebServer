 /// @file    util.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-18 18:45:21
 
#include "base/logging.h"
#include "util.h"
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define ErrorCheck(ret, error, name) \
do{\
	if(ret == error){\
		perror(name);\
		exit(0);\
	}\
}while(0)

int socket_bind_listen(const InetAddress &listenAddr)
{
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	ErrorCheck(listenFd, -1, "socket");

	//ReuseAddr
	int opt = 1;
	socklen_t len = static_cast<socklen_t>(sizeof(opt));
	int ret = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, len);
	ErrorCheck(ret, -1, "setsockopt");

	ret = bind(listenFd, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
	ErrorCheck(ret, -1, "bind");

	ret = listen(listenFd, 1000);
	ErrorCheck(ret, -1, "listen");

	return listenFd;
}

void setSocketNonBlocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flag);
}

void setSocketNoDelay(int fd, bool start)
{
	int enable = start ? true : false;
	int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
	ErrorCheck(ret, -1, "setsockopt");
}

void setSocketKeepAlive(int fd, bool start)
{
	int enable = start ? true : false;
	int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&enable, sizeof(enable));
	ErrorCheck(ret, -1, "setsockopt");
}

int getSockError(int fd)
{
	int optval;
	socklen_t len = sizeof(optval);
	if( getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &len) < 0 ){
		return errno;
	}
	else{
		return optval;
	}
}

void shutdownWrite(int fd)
{
	::shutdown(fd, SHUT_WR);
}

struct sockaddr_in getLocalAddr(int sockfd)
{
	struct sockaddr_in localaddr;
	bzero(&localaddr, sizeof(localaddr));
	socklen_t addrlen = sizeof(localaddr);
	if(::getsockname(sockfd, (struct sockaddr*)&localaddr, &addrlen) < 0){
		LOG_SYSERR << "getLocalAddr error";
	}
	return localaddr;
}

struct sockaddr_in getPeerAddr(int sockfd)
{
	struct sockaddr_in peerAddr;
	bzero(&peerAddr, sizeof(peerAddr));
	socklen_t addrlen = sizeof(peerAddr);
	if(::getpeername(sockfd, (struct sockaddr*)&peerAddr, &addrlen) < 0){
		LOG_SYSERR << "getPeerAddr error";
	}
	return peerAddr;
}
