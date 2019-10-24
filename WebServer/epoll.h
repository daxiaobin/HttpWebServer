 /// @file    epoll.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-28 18:20:17
 
#ifndef EPOLL_H__
#define EPOLL_H__

#include "base/timestamp.h"
#include "eventloop.h"
#include <vector>
#include <map>
#include <sys/epoll.h>

class Channel;

class Epoll
{
public:
	typedef std::vector<Channel*> ChannelList;

	Epoll(EventLoop *loop);
	~Epoll();

	Timestamp epoll(int timeoutMs, ChannelList *activeChannels);
	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);
private:
	void update(int operation, Channel *channel);
private:
	EventLoop *ownerLoop_;
	int epollfd_;
	std::vector<struct epoll_event> events_;
	std::map<int, Channel*> Channels_;
};


#endif
