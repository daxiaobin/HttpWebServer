 /// @file    eventloopthreadpool.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-27 20:50:58
 
#include "eventloopthreadpool.h"
#include "eventloopthread.h"
#include "eventloop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop)
: baseLoop_(baseloop)
, started_(false)
, numThreads_(0)
, next_(0)
{}

void EventLoopThreadPool::start()
{
	baseLoop_->assertInLoopThread();
	started_ = true;

	for(int i = 0; i < numThreads_; ++i){
		std::shared_ptr<EventLoopThread> t = std::make_shared<EventLoopThread>();
		threads_.push_back(t);
		loops_.push_back(t->startLoop());
	}
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	EventLoop *loop = baseLoop_; //这里必须初始化为baseLoop_，因为有可能不需要线程池，这种情况下直接返回baseLoop_
	if(!loops_.empty()){
		loop = loops_[next_];
		++next_;
		if(static_cast<size_t>(next_) >= loops_.size()){
			next_ = 0;
		}
	}
	return loop;
}
