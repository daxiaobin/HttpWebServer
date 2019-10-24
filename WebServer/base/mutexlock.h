 /// @file    mutexlock.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-06 14:42:32
 
#ifndef MUTEXLOCK_H__
#define MUTEXLOCK_H__


#include "noncopyable.h"
#include <pthread.h>

class MutexLock : Noncopyable{
	public:
		MutexLock()
		{
			pthread_mutex_init(&_mutex, NULL);
		}
		~MutexLock()
		{
			pthread_mutex_destroy(&_mutex);
		}

		void lock(){
			pthread_mutex_lock(&_mutex);
		}
		void unlock(){
			pthread_mutex_unlock(&_mutex);
		}

		pthread_mutex_t& getMutexLock(){
			return _mutex;
		}
	private:
		pthread_mutex_t _mutex;
};

class MutexLockGuard : Noncopyable{
	public:
		explicit MutexLockGuard(MutexLock &mutex)
		: _mutex(mutex)
		{
			_mutex.lock();
		}

		~MutexLockGuard()
		{
			_mutex.unlock();
		}
	private:
		MutexLock &_mutex;
};
#define MutexLockGuard(x) static_assert(false, "miss MutexLockGuard var name")

#endif
