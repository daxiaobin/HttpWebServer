 /// @file    acceptor.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-18 13:32:12
 
#ifndef ACCEPTOR_H__
#define ACCEPTOR_H__


#include "base/noncopyable.h"
#include "channel.h"
#include "inetaddress.h"
#include <functional>

class EventLoop;
class Acceptor : Noncopyable
{
public:
	typedef std::function<void(int, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop *loop, const InetAddress &listenAddr);
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
	int listenFd_;
	Channel channel_;
	NewConnectionCallback newConnectionCallback_;
};



#endif
