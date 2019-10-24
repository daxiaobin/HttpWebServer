 /// @file    timerqueue.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-07 15:09:11
 
#ifndef TIMERQUEUE_H__
#define TIMERQUEUE_H__

#include "base/timestamp.h"
#include "base/noncopyable.h"
#include "channel.h"
#include <memory>
#include <set>
#include <vector>

//定时器的处理逻辑：
//	添加定时器：
//		首先将定时器insert到multiset中，然后根据multiset中是否为空或者新添加的定时器到期时间是否小于multiset中已有所有定时器的到期时间，
//		如果是，就要启动定时器。至于为什么要这样要了解timerfd的使用，也就是如果在启动定时器后未到期以前又重新设置了定时器，那么定时器的到期时间
//		将会被改变，所以要一个一个的启动定时器，每次启动最先到期的那个定时器。
//		此外要注意，在调用addTimer和调用addTimerInLoop这两个函数间会存在延迟，因此在启动定时器时要根据 现在的时间--到期的时间 之差来设置到期时间。
//	处理到期定时器：
//		由于multiset是有序的，所以当有定时器到期后一定是最前面的定时器，又因为定时器的到期时间可能有相同的，因此这里使用multiset，并且取出到期的
//		定时器是通过lower_bound成员函数。注意lower_bound的参数的TimerNode是晚于定时器到期的那个时刻的。
//		取出到期的定时器之后放入vector中，然后逐个执行回调函数，执行完之后要处理已到期的定时器。处理过程是：若这个定时器是间隔式定时器，则重新将这个
//		定时器插入到multiset中，非间隔式定时器已在上面移除multiset了，所以不需要关注。将所有的间隔式定时器插入multiset中后，此时是没有定时器启动了的，
//		所以要开始启动multiset中的第一个定时器。至此逻辑流程处理完毕。
class TimerNode{
public:
	typedef std::function<void()> TimerCallback;

	explicit TimerNode(Timestamp expiration)
	: expiration_(expiration)
	, interval_(0)
	, repeat_(false)
	{}
	TimerNode(const TimerCallback &cb, Timestamp expiration, double interval)
	: cb_(cb)
	, expiration_(expiration)
	, interval_(interval)
	, repeat_(interval > 0)
	{}

	void run() const { cb_(); }
	Timestamp expiration() const { return expiration_; }
	bool repeat() const { return repeat_; }

	void restart(Timestamp now);

	bool operator<(const TimerNode &rhs) const{ //这里必须加const，因为常量对象只能调用const成员函数
		return expiration_ < rhs.expiration_;
	}
	bool operator==(const TimerNode &rhs) const{
		return expiration_ == rhs.expiration_;
	}
private:
	const TimerCallback cb_;
	Timestamp expiration_;
	const double interval_;
	const bool repeat_;
};

class TimerQueue : Noncopyable
{
public:
	typedef std::function<void()> TimerCallback;
	typedef std::multiset<TimerNode> TimerList;

	TimerQueue(EventLoop *loop);
	~TimerQueue();

	void handleRead();
	void addTimer(const TimerCallback &cb, Timestamp when, double interval);
private:
	void addTimerInLoop(TimerNode timernode);
	int createTimerfd();
	bool insert(TimerNode &timernode);
	void startTimer(TimerNode &timernode);
	std::vector<TimerNode> getExpired(TimerNode &sentry);
	void handleExpiredTimer(std::vector<TimerNode> &expired);
private:
	EventLoop *loop_;
	const int timerfd_;
	Channel timerfdChannel_;
	TimerList timers_; //只会在loop_线程进行写操作，不要加锁
};


#endif
