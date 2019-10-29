 /// @file    eventloopthreadpool.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-27 20:40:21
 
#ifndef EVENTLOOPTHREADPOOL_H__
#define EVENTLOOPTHREADPOOL_H__ 

#include "base/condition.h"
#include "base/mutexlock.h"
#include "base/thread.h"
#include "base/noncopyable.h"

#include <vector>
#include <functional>
#include <memory>

class EventLoop;
class EventLoopThread;

//这个类是Server类的成员变量，Server类只在主线程中创建一个对象，
//所以这个线程池类也只在主线程中创建一个对象，然后依次创建其他的I/O线程。
class EventLoopThreadPool : Noncopyable
{
public:
	EventLoopThreadPool(EventLoop *baseloop);

	void start();
	EventLoop *getNextLoop();

	void setThreadNum(int numThreads) { numThreads_ = numThreads; }
private:
	EventLoop *baseLoop_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::shared_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};


#endif
