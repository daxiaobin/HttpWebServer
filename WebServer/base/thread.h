 /// @file    thread.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-16 10:50:39
 
#ifndef THREAD_H__
#define THREAD_H__

#include "noncopyable.h"
#include <functional>
#include <string>

using std::string;

class Thread : Noncopyable
{
public:
	typedef std::function<void()> ThreadFunc;

	explicit Thread(const ThreadFunc &, const string &name = string());
	~Thread();

	void start();
	void join();

	bool started() const { return started_; }
	pid_t tid() const { return tid_; }
	const string& name() const { return name_; }
private:
	bool started_;
	bool joined_;
	pthread_t pthID_;
	pid_t tid_;
	ThreadFunc func_;
	string name_;
};

namespace CurrentThread
{
	pid_t tid();
	const char* name();
	bool isMainThread();
}



#endif
