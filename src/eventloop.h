 /// @file    eventloop.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-16 18:51:37
 
#ifndef EVENTLOOP_H__
#define EVENTLOOP_H__

#include "base/noncopyable.h"
#include "base/thread.h"
#include "base/mutexlock.h"
#include "timerqueue.h"
#include <unistd.h>
#include <memory>
#include <functional>
#include <vector>

class Epoll;
class Channel;
class EventLoop : Noncopyable
{
public:
	typedef std::function<void()> Functor;
	typedef std::function<void()> TimerCallback;

	EventLoop();
	~EventLoop();

	void loop();
	void quit();
	bool isInLoopThread() const { return tid_ == CurrentThread::tid(); }
	void assertInLoopThread()
	{
		if(!isInLoopThread()){
			abortNotInLoopThread();
		}
	}

	void runAt(const Timestamp &time, const TimerCallback &cb);
	void runAfter(double delay, const TimerCallback &cb);
	void runEvery(double interval, const TimerCallback &cb);

	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

	void runInLoop(const Functor &cb); //用于线程间的调配任务
	void queueInLoop(const Functor &cb); //如果不是在EventLoop对象所在的线程调用runInLoop()，则转而调用queueInLoop()
	void wakeup(); //用来唤醒EventLoop对象所在的I/O线程，因为I/O线程会阻塞在IO多路复用的调用上，并且也只会在这里阻塞
private:
	void abortNotInLoopThread() const{
		abort();
	}
	void handleRead(); //用于处理由wakeup()唤醒后读事件描述符
	void doPendingFunctors(); //和handleRead()类似，但是前者只是读eventfd，而这个函数是真正的调用该eventfd唤醒后所要做的事情
private:
	typedef std::vector<Channel*> ChannelList;

	bool looping_;
	bool quit_;
	bool callingPendingFunctors_;
	pid_t tid_;
	std::unique_ptr<Epoll> poller_; //Poller对象唯一被EventLoop对象使用
	std::unique_ptr<TimerQueue> timerqueue_;
	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;
	MutexLock mutex_;
	std::vector<Functor> pendingFunctors_;
	ChannelList activeChannels_;
};




#endif
