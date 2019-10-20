 /// @file    acceptor.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-18 13:32:12
 
#ifndef ACCEPTOR_H__
#define ACCEPTOR_H__


#include "base/noncopyable.h"
#include "channel.h"
#include <functional>

class EventLoop;
class Acceptor : Noncopyable
{
public:
	typedef std::function<void(int)> NewConnectionCallback;

	Acceptor(EventLoop *loop, unsigned short port);
	~Acceptor();

	void setConnectionCallback(const NewConnectionCallback &cb)
	{ newConnectionCallback_ = cb; }
	bool listenning() const { return listenning_; }

	void listen();
private:
	void handleRead();
private:
	bool listenning_;
	EventLoop *loop_;
	unsigned short port_;
	int listenFd_;
	Channel channel_;
	NewConnectionCallback newConnectionCallback_;
};



#endif
