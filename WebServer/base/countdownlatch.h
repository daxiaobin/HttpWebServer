 /// @file    countdownlatch.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-12 09:10:41
 
#ifndef COUNTDOWNLATCH_H__
#define COUNTDOWNLATCH_H__


#include "noncopyable.h"
#include "mutexlock.h"
#include "condition.h"
//倒计时类
class CountDownLatch : Noncopyable{
public:
	explicit CountDownLatch(int count);

	void wait();
	void countDown();
	int getCount() const { 
		return count_; 
	}
private:
	MutexLock mutex_;
	Condition cond_;
	int count_;
};


#endif
