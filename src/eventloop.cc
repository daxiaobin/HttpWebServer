 /// @file    eventloop.c
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-17 15:21:48
 
#include "eventloop.h"
#include "epoll.h"
#include "channel.h"
#include <signal.h>
#include <assert.h>
#include <sys/eventfd.h>

__thread EventLoop *t_loopInThisThread = nullptr;
const int kPollTimeMs = 10000; //用于poll的超时时间

static int createEventFd()
{
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(evtfd < 0){
		perror("eventfd error");
		abort();
	}
	return evtfd;
}

class IgnoreSigpipe
{
public:
	IgnoreSigpipe(){
		::signal(SIGPIPE, SIG_IGN);
	}

};

IgnoreSigpipe initObj;

EventLoop::EventLoop()
: looping_(false)
, quit_(false)
, callingPendingFunctors_(false)
, tid_(CurrentThread::tid())
, poller_(new Epoll(this))
, timerqueue_(new TimerQueue(this))
, wakeupFd_(createEventFd())
, wakeupChannel_(new Channel(this, wakeupFd_))
{
	if(t_loopInThisThread){
		abortNotInLoopThread();
	}
	else{
		t_loopInThisThread = this;
	}
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
	assert(!looping_);
	t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;
	quit_ = false;

	while(!quit_){
		activeChannels_.clear();
		Timestamp pollReturnTime = poller_->epoll(kPollTimeMs, &activeChannels_);
		for(ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it){
			(*it)->handleEvent(pollReturnTime);
		}
		doPendingFunctors();
	}

	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
	if(!isInLoopThread()){ //跨线程调用时
		wakeup();
	}
}

void EventLoop::runInLoop(const Functor &cb)
{
	if(isInLoopThread()){
		cb();
	}
	else{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor &cb)
{
	{
		MutexLockGuard autoLock(mutex_);
		pendingFunctors_.push_back(cb);
	}
	//当由其他线程调用这个函数时要进行wakeup唤醒，当本线程自己调用这个函数时如果callingPendingFunctors_为true也要wakeup唤醒，
	//因为如果不唤醒那么在doPendingFunctor()结束后这个cb就得不到及时的处理。
	if(!isInLoopThread() || callingPendingFunctors_){
		wakeup();
	}
}

void EventLoop::runAt(const Timestamp &time, const TimerCallback &cb)
{
	timerqueue_->addTimer(cb, time, 0.0);
}

void EventLoop::runAfter(double delay, const TimerCallback &cb)
{
	Timestamp time(Timestamp::addTime(Timestamp::now(), delay));
	runAt(time, cb);
}

void EventLoop::runEvery(double interval, const TimerCallback &cb)
{
	Timestamp time(Timestamp::addTime(Timestamp::now(), interval));
	timerqueue_->addTimer(cb, time, interval);
}

void EventLoop::updateChannel(Channel *channel)
{
	assertInLoopThread();
	poller_->updateChannel(channel);
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t ret = ::write(wakeupFd_, &one, sizeof(one));
	if(ret != sizeof(one)){
		
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 0;
	ssize_t ret = ::read(wakeupFd_, &one, sizeof(one));
	if(ret != sizeof(one)){

	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> Functors;
	callingPendingFunctors_ = true;

	{ 
		MutexLockGuard autoLock(mutex_);
		Functors.swap(pendingFunctors_); //使用swap可以缩小临界区范围，之后的操作在局部变量中进行。
	}

	for(size_t i = 0; i != Functors.size(); ++i){
		Functors[i]();
	}

	callingPendingFunctors_ = false;
}

void EventLoop::removeChannel(Channel *channel)
{
	assertInLoopThread();
	poller_->removeChannel(channel);
}
