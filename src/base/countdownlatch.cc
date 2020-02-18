 /// @file    countdownlatch.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-12 09:14:02
 
#include "countdownlatch.h"

CountDownLatch::CountDownLatch(int count)
: mutex_()
, cond_(mutex_)
, count_(count)
{}

void CountDownLatch::wait()
{
	MutexLockGuard autoLock(mutex_);
	while(count_ > 0){
		cond_.wait();
	}
}

void CountDownLatch::countDown()
{
	MutexLockGuard autoLock(mutex_);
	--count_;
	if(count_ == 0){
		cond_.notifyall();
	}
}
