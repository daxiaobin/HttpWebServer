 /// @file    eventloopthread.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-17 20:15:32
 
#include "eventloopthread.h"
#include <functional>
#include "eventloop.h"

EventLoopThread::EventLoopThread()
: loop_(nullptr)
, exiting_(false)
, thread_(std::bind(&EventLoopThread::threadFunc, this))
, mutex_()
, cond_(mutex_)
{}

EventLoopThread::~EventLoopThread()
{
	exiting_ = true;
	loop_->quit();
	thread_.join();
}

//这个函数是主线程调用的。
//该函数返回EventLoop对象的指针，因此创建的所有线程中唯一的EventLoop对象都可被主线程访问。
EventLoop* EventLoopThread::startLoop()
{
	thread_.start();
	{
		MutexLockGuard autoLock(mutex_);
		while(loop_ == nullptr){
			cond_.wait();
		}
	}
	return loop_;
}

//这个函数是创建的I/O线程调用的
void EventLoopThread::threadFunc()
{
	EventLoop loop;
	{
		MutexLockGuard autoLock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}
	loop.loop();
}
