 /// @file    asynclogging.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-12 08:54:35
 
#ifndef ASYNCLOGGING_H__
#define ASYNCLOGGING_H__


#include "noncopyable.h"
#include "thread.h"
#include "mutexlock.h"
#include "condition.h"
#include "countdownlatch.h"
#include "logstream.h"

#include <atomic>
#include <vector>
#include <memory>

//异步日志理解：用一个背景线程（后端线程）负责收集日志消息，并写入日志文件，其他业务线程（前端线程）只管往这个背景线程发送日志消息
//				这就是所谓的异步日志。

class AsyncLogging : Noncopyable{
public:
	AsyncLogging(const std::string &basename, off_t rollSize, int flushInterval = 3);
	~AsyncLogging();

	void append(const char *logline, size_t len);
	void start();
	void stop();
private:
	void threadFunc();
private:
	typedef FixedBuffer<kLargeBuffer> Buffer;
	typedef std::vector<std::unique_ptr<Buffer>> BufferPtrVector;
	typedef BufferPtrVector::value_type BufferPtr;

	const std::string basename_;
	off_t rollSize_;
	const int flushInterval_;
	std::atomic<bool> running_;
	Thread thread_;
	CountDownLatch latch_;
	MutexLock mutex_;
	Condition cond_;
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	BufferPtrVector buffers_;
};


#endif
