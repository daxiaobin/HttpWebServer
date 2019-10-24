 /// @file    util.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-18 14:15:17


#ifndef UTIL_H__
#define UTIL_H__

 
int socket_bind_listen(unsigned short port);
void setSocketNonBlocking(int fd);
void setSocketNoDelay(int fd, bool start);
void setSocketKeepAlive(int fd, bool start);
int getSockError(int fd);
void shutdownWrite(int fd);


#endif
