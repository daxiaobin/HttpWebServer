 /// @file    util.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-18 14:15:17


#ifndef UTIL_H__
#define UTIL_H__

#include "inetaddress.h"
#include <arpa/inet.h>

//FIXME 以下函数放入一个命名空间中比较合适
 
int socket_bind_listen(const InetAddress &listenAddr);
void setSocketNonBlocking(int fd);
void setSocketNoDelay(int fd, bool start);
void setSocketKeepAlive(int fd, bool start);
int getSockError(int fd);
void shutdownWrite(int fd);

struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);
#endif
