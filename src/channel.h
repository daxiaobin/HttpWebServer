 /// @file    channel.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-17 09:01:44
 
#ifndef CHANNEL_H__
#define CHANNEL_H__

#include "base/noncopyable.h"
#include "base/timestamp.h"
#include <functional>

//对外接口是set*Callback()函数和enable*()函数。
class EventLoop;
class Channel : Noncopyable
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;

	Channel(EventLoop *loop, int fd);

	void handleEvent(Timestamp receiveTime);
	void setReadCallback(const ReadEventCallback &cb)
	{ readCallback_ = cb; }
	void setWriteCallback(const EventCallback &cb)
	{ writeCallback_ = cb; }
	void setErrorCallback(const EventCallback &cb)
	{ errorCallback_ = cb; }
	void setCloseCallback(const EventCallback &cb)
	{ closeCallback_ = cb; }

	int fd() const { return fd_; }
	int events() const { return events_; }
	void set_revents(int rev) { revents_ = rev; } 
	bool isNoneEvent() const { return events_ == 0; }

	void disableAll() { events_ = kNoneEvent; update(); }
	void disableWrite() { events_ &= ~kWriteEvent; update(); }

	void enableReading() { events_ |= kReadEvent; update(); }
	void enableWriting() { events_ |= kWriteEvent; update(); }

	bool isWriting() { return events_ & kWriteEvent; }

	EventLoop* ownerLoop() const { return loop_; }

	//for Poller::updateChannel()
	int state() const { return state_; }
	void set_state(int state) { state_ = state; }
private:
	void update();

	static const int kReadEvent;
	static const int kWriteEvent;
	static const int kNoneEvent;
private:
	EventLoop *loop_;
	int fd_;
	int events_;
	int revents_;
	// for Poller
	int state_;

	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback errorCallback_;
	EventCallback closeCallback_;
};


#endif
