 /// @file    thread.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-14 09:50:58
 
#include "thread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>


__thread int t_cacheTid = 0;
__thread const char* t_threadName = "unknown";

namespace CurrentThread
{
	pid_t tid(){
		if(t_cacheTid == 0){
			t_cacheTid = static_cast<pid_t>(syscall(SYS_gettid));
			return t_cacheTid;
		}
		else{
			return t_cacheTid;
		}
	}

	const char* name(){
		return t_threadName;
	}

	bool isMainThread(){
		return tid() == getpid();
	}
}

struct ThreadData{
	typedef Thread::ThreadFunc ThreadFunc;
	ThreadFunc func_;
	pid_t* tid_;
	std::string name_;

	ThreadData(const ThreadFunc &func, pid_t* tid, const std::string& name)
	: func_(func)
	, tid_(tid)
	, name_(name)
	{}

	void runInThread(){
		*tid_ = CurrentThread::tid();
		tid_ = nullptr;
		t_threadName = name_.empty() ? "unknown" : name_.c_str();

		func_();
	}

};

//线程主函数
void* startThread(void *arg)
{
	ThreadData *data = static_cast<ThreadData*>(arg);
	data->runInThread();
	delete data;
	return nullptr;
}

Thread::Thread(const ThreadFunc &func, const std::string &name)
: started_(false)
, joined_(false)
, pthID_(0)
, tid_(0)
, func_(func)
, name_(name)
{}

Thread::~Thread()
{
	if(started_ && !joined_){
		pthread_detach(pthID_);
	}
}

void Thread::start()
{
	assert(!started_);
	started_ = true;
	ThreadData *data = new ThreadData(func_, &tid_, name_);
	if(pthread_create(&pthID_, NULL, startThread, data) != 0){ //创建线程若失败
		started_ = false;
		delete data;
	}
}

void Thread::join()
{
	assert(started_);
	assert(!joined_);
	joined_ = true;
	pthread_join(pthID_, NULL);
}
