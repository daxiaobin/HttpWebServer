 /// @file    timerqueue.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-07 15:33:01
 
#include "timerqueue.h"
#include "eventloop.h"
#include <sys/timerfd.h>
#include <string.h>

void TimerNode::restart(Timestamp now)
{
	if(repeat_){
		expiration_ = Timestamp::addTime(now, interval_);
	}
	else{
		expiration_ = Timestamp::invalid(); //一个空的Timestamp对象
	}
}

TimerQueue::TimerQueue(EventLoop *loop)
: loop_(loop)
, timerfd_(createTimerfd())
, timerfdChannel_(loop_, timerfd_)
{
	timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
	close(timerfd_);
}

int TimerQueue::createTimerfd()
{
	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if(timerfd < 0){

	}
	return timerfd;
}

void TimerQueue::addTimer(const TimerCallback &cb, Timestamp when, double interval)
{
	TimerNode timernode(cb, when, interval);
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timernode));
}

void TimerQueue::addTimerInLoop(TimerNode timernode)
{
	loop_->assertInLoopThread();
	bool earliest = insert(timernode);
	if(earliest){
		startTimer(timernode);
	}
}

bool TimerQueue::insert(TimerNode &timernode)
{
	bool earliest = false;
	Timestamp when = timernode.expiration();
	TimerList::iterator it = timers_.begin();
	if(it == timers_.end() || when < it->expiration()){
		earliest = true;
	}
	timers_.insert(timernode);
	return earliest;
}

void TimerQueue::startTimer(TimerNode &timernode)
{
	struct itimerspec newValue;
	bzero(&newValue, sizeof(newValue));
	int64_t microseconds = timernode.expiration().microsecondsSinceEpoch() - Timestamp::now().microsecondsSinceEpoch();
	if(microseconds < 1000){
		microseconds = 1000;
	}
	newValue.it_value.tv_sec = microseconds / Timestamp::kMicrosecondsPerSecond;
	newValue.it_value.tv_nsec = (microseconds % Timestamp::kMicrosecondsPerSecond) * 1000;
	int ret = timerfd_settime(timerfd_, 0, &newValue, NULL);
	if(ret == -1){

	}
}

void TimerQueue::handleRead()
{
	loop_->assertInLoopThread();
	uint64_t count;
	size_t n = read(timerfd_, &count, sizeof(count));
	if(n != sizeof(count)){
		
	}
	
	TimerNode sentry(Timestamp::now());
	std::vector<TimerNode> expired = getExpired(sentry);
	for(auto &mem : expired){
		mem.run();
	}
	handleExpiredTimer(expired);
}

std::vector<TimerNode> TimerQueue::getExpired(TimerNode &sentry)
{
	std::vector<TimerNode> expired;
	TimerList::iterator it = timers_.lower_bound(sentry);
	std::copy(timers_.begin(), it, back_inserter(expired));
	timers_.erase(timers_.begin(), it);
	return expired;
}

void TimerQueue::handleExpiredTimer(std::vector<TimerNode> &expired)
{
	Timestamp now(Timestamp::now());
	for(auto &mem : expired){
		if(mem.repeat()){
			mem.restart(now);
			insert(mem);
		}
	}
	if(!timers_.empty()){
		TimerNode nextExpire = *timers_.begin();
		if(nextExpire.expiration().valid()){
			startTimer(nextExpire);
		}
	}
}
