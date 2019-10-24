 /// @file    condition.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-06 15:01:59
 
#ifndef CONDITION_H__
#define CONDITION_H__

#include "noncopyable.h"
#include "mutexlock.h"
#include <time.h>
#include <errno.h>

class Condition : Noncopyable{
	public:
		explicit Condition(MutexLock &mutex)
		: _mutex(mutex)
		{
			pthread_cond_init(&_cond, NULL);
		}
		~Condition(){
			pthread_cond_destroy(&_cond);
		}

		void notify(){
			pthread_cond_signal(&_cond);
		}
		void notifyall(){
			pthread_cond_broadcast(&_cond);
		}
		void wait(){
			pthread_cond_wait(&_cond, &_mutex.getMutexLock());
		}
		bool waitForSeconds(int seconds){
			struct timespec abstime;
			clock_gettime(CLOCK_REALTIME, &abstime);
			abstime.tv_sec += static_cast<time_t>(seconds);
			return ETIMEDOUT == pthread_cond_timedwait(&_cond, &_mutex.getMutexLock(), &abstime);
		}
	private:
		MutexLock &_mutex;
		pthread_cond_t _cond;
};


#endif
