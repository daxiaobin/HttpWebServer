 /// @file    eventloopthread.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-17 20:11:47
 
#ifndef EVENTLOOPTHREAD_H__
#define EVENTLOOPTHREAD_H__

#include "base/noncopyable.h"
#include "base/mutexlock.h"
#include "base/condition.h"
#include "base/thread.h"

class EventLoop;
class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop* startLoop();
private:
	void threadFunc();
private:
	EventLoop *loop_;
	bool exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
};


#endif
