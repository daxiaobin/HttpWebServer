 /// @file    epoll.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-28 18:20:23
 
#include "epoll.h"
#include "channel.h"
#include "base/logging.h"
#include <assert.h>

static const int kNew = -1;
static const int kAdded = 0;
static const int kDeleted = 1;

static const int kInitEventListSize = 16;

Epoll::Epoll(EventLoop *loop)
: ownerLoop_(loop)
, epollfd_(::epoll_create(5))
, events_(kInitEventListSize)
{
	assert(epollfd_ > 0);
}

Epoll::~Epoll()
{
	::close(epollfd_);
}

//epoll_wait返回的最大值为events_.size()的大小
Timestamp Epoll::epoll(int timeoutMs, ChannelList *activeChannels)
{
	int numEvents = ::epoll_wait(epollfd_, events_.data(), events_.size(), timeoutMs);
	Timestamp now(Timestamp::now());
	if(numEvents > 0){
		for(int i = 0; i < numEvents; ++i){
			Channel *channel = (Channels_.find(events_[i].data.fd))->second;
			channel->set_revents(events_[i].events);
			activeChannels->push_back(channel);
		}
		if(static_cast<size_t>(numEvents) == events_.size()){
			events_.resize(numEvents * 2);
		}
	}
	else if(numEvents == 0){
		//LOG_TRACE << "nothing happended";
	}
	else{
		LOG_SYSERR << "epoll_wait() error";
	}
	return now;
}

//channel所管理的文件描述符的注册状态只会在这里被改变
void Epoll::updateChannel(Channel *channel)
{
	ownerLoop_->assertInLoopThread();
	int state = channel->state();
	if(state == kNew || state == kDeleted){ //如果该文件描述符是第一次注册或者之前被删除过，则这次是再次重新注册
		Channels_[channel->fd()] = channel;
		channel->set_state(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else{ //如果该文件描述符已经注册过，则表明这个文件描述符现在是要修改或者是删除
		if(channel->isNoneEvent()){
			update(EPOLL_CTL_DEL, channel);
			channel->set_state(kDeleted);
		}
		else{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void Epoll::update(int operation, Channel *channel)
{
	struct epoll_event evt;
	evt.events = channel->events();
	evt.data.fd = channel->fd();
	if(::epoll_ctl(epollfd_, operation, channel->fd(), &evt) < 0){
		printf("epoll_ctl error\n");
	}
}

void Epoll::removeChannel(Channel *channel)
{
	ownerLoop_->assertInLoopThread();
	size_t n = Channels_.erase(channel->fd());
	assert(n == 1);
	(void)n;
	if(channel->state() == kAdded){
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_state(kDeleted);
}
