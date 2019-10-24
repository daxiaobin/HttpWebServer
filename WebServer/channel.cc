 /// @file    channel.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-17 09:35:01

#include "channel.h"
#include <sys/epoll.h>
#include "eventloop.h"

const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;
const int Channel::kNoneEvent = 0;

Channel::Channel(EventLoop *loop, int fd)
: loop_(loop)
, fd_(fd)
, events_(0)
, revents_(0)
, state_(-1)
{}

void Channel::handleEvent(Timestamp receiveTime)
{
	if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
		if(closeCallback_)
			closeCallback_();
	}
	if(revents_ & (EPOLLERR)){
		if(errorCallback_)	
			errorCallback_();
	}
	if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
		if(readCallback_)
			readCallback_(receiveTime);
	}
	if(revents_ & EPOLLOUT){
		if(writeCallback_)
			writeCallback_();
	}
}

void Channel::update()
{
	loop_->updateChannel(this);
}
