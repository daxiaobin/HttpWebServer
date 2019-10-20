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
