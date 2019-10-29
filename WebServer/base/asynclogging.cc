 /// @file    asynclogging.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-12 09:54:34
 
#include "asynclogging.h"
#include "logfile.h"
#include "timestamp.h"	
#include <functional>

AsyncLogging::AsyncLogging(const std::string &basename, off_t rollSize, int flushInterval)
: basename_(basename)
, rollSize_(rollSize)
, flushInterval_(flushInterval)
, running_(false)
, thread_(std::bind(&AsyncLogging::threadFunc, this), "logging")
, latch_(1)
, mutex_()
, cond_(mutex_)
, currentBuffer_(new Buffer)
, nextBuffer_(new Buffer)
{
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
	if(running_){
		stop();
	}
}

void AsyncLogging::start()
{
	running_ = true;
	thread_.start();
	latch_.wait();
}

void AsyncLogging::stop()
{
	running_ = false;
	cond_.notify();
	thread_.join();
}

void AsyncLogging::append(const char *logline, size_t len)
{
	MutexLockGuard autoLock(mutex_); //这里如果业务线程太多会造成锁争用，会有点影响性能。
	if(currentBuffer_->avail() > len){ //最常发生的情况
		currentBuffer_->append(logline, len);
	}
	else{
		buffers_.push_back(std::move(currentBuffer_)); //这里必须是move，因为添加进容器是发生拷贝，而unique_ptr不能发生拷贝
		if(nextBuffer_){
			currentBuffer_ = std::move(nextBuffer_); //同样这里也必须用move
		}
		else{
			currentBuffer_.reset(new Buffer); //如果前端线程写入的太快，一下子把两个缓冲区都用完了就发生这种情况，这是很少发生的情况
		}
		currentBuffer_->append(logline, len);
		cond_.notify(); //通知后端线程开始写日志到文件中去
	}
}

//当创建一个后端线程的时候就运行这个函数
void AsyncLogging::threadFunc() 
{
	latch_.countDown();
	LogFile output(basename_, rollSize_); //在这里创建LogFile对象。
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferPtrVector bufferToWrite;
	bufferToWrite.reserve(16);
	while(running_){
		{ //这一语句块是核心内容
			MutexLockGuard autoLock(mutex_);
			if(buffers_.empty()){ //这里不能用while，因为是waitForSeconds，被唤醒后buffers_仍然可能是empty
				cond_.waitForSeconds(flushInterval_);
			}
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_ = std::move(newBuffer1);
			bufferToWrite.swap(buffers_);
			if(!nextBuffer_){
				nextBuffer_ = std::move(newBuffer2);
			}
		}

		if(bufferToWrite.size() > 25){ //当前端线程发生异常，一下子写的太多时
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
			Timestamp::now().toFormattedString().c_str(),
			bufferToWrite.size()-2);
			fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			bufferToWrite.erase(bufferToWrite.begin()+2, bufferToWrite.end());	
		}

		for(auto &mem : bufferToWrite){ //mem为unique_ptr<FixedBuffer<kLargeBuffer> >类型
			output.append(mem->data(), mem->length());
		}

		if(bufferToWrite.size() > 2){
			bufferToWrite.resize(2); //删除多余的缓冲区，多余的来源于append函数new出来的Buffer
		}

		if(!newBuffer1){ //newBuffer1肯定是为空的，否则没有东西可以写入日志文件中
			newBuffer1 = std::move(bufferToWrite.back());
			bufferToWrite.pop_back();
			newBuffer1->reset(); //调用FixedBuffer<kLargeBuffer>的reset函数
		}

		if(!newBuffer2){
			newBuffer2 = std::move(bufferToWrite.back());
			bufferToWrite.pop_back();
			newBuffer2->reset();
		}

		bufferToWrite.clear();
		output.flush();
	}
	output.flush();
}
