 /// @file    connection.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-21 21:27:22
 
#include "connection.h"
#include "eventloop.h"
#include "channel.h"
#include "util.h"
#include "base/logging.h"
#include <assert.h>

using std::placeholders::_1;

Connection::Connection(EventLoop *loop, const std::string &name, int connfd, const InetAddress &localaddr, const InetAddress &peerAddr)
: loop_(loop)
, connfd_(connfd)
, name_(name)
, channel_(new Channel(loop_, connfd))
, state_(kConnecting)
, localaddr_(localaddr)
, peerAddr_(peerAddr)
{
	channel_->setReadCallback(std::bind(&Connection::handleRead, this, _1));
	channel_->setWriteCallback(std::bind(&Connection::handleWrite, this));
	channel_->setCloseCallback(std::bind(&Connection::handleClose, this));
	channel_->setErrorCallback(std::bind(&Connection::handleError, this));
}

Connection::~Connection()
{
	::close(connfd_);
}

//this function should be called only once
void Connection::connectionEstablished()
{
	loop_->assertInLoopThread();
	state_ = kConnected;
	channel_->enableReading();

	connectionCallback_(shared_from_this());
}

void Connection::connectionDestroyed()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnected || state_ == kDisconnecting);
	setState(kDisconnected);
	channel_->disableAll();
	connectionCallback_(shared_from_this());
	loop_->removeChannel(channel_.get()); //FIXME 这样写不好，不应该返回裸指针，但是也不会发生内存泄漏，因为其他地方没有delete。可以改为共享指针。
}

void Connection::send(const std::string &message)
{
	if(state_ == kConnected){
		if(loop_->isInLoopThread()){
			sendInLoop(message);
		}
		else{
			loop_->runInLoop(std::bind(&Connection::sendInLoop, this, message));
		}
	}
}

void Connection::sendInLoop(const std::string &message)
{
	loop_->assertInLoopThread();
	ssize_t nwrite = 0;
	//if no thing in output queue, try writing directly.
	if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
		nwrite = ::write(connfd_, message.data(), message.size());
		if(nwrite >= 0){
			if(static_cast<size_t>(nwrite) < message.size()){
				printf("I am going to write more data\n");
			}
		}
		else{
			nwrite = 0;
			if(errno != EAGAIN || errno != EWOULDBLOCK){
				
			}
		}
	}

	if(static_cast<size_t>(nwrite) < message.size()){
		outputBuffer_.append(message.data() + nwrite, message.size() - nwrite);
		if(!channel_->isWriting()){
			channel_->enableWriting();
		}
	}
}

void Connection::shutdown()
{
	if(state_ == kConnected){
		state_ = kDisconnecting;
		loop_->runInLoop(std::bind(&Connection::shutdownInLoop, shared_from_this()));
	}
}

void Connection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if(!channel_->isWriting()){
		util::shutdownWrite(connfd_);
	}
}

void Connection::setTcpNoDelay(bool start)
{
	util::setSocketNoDelay(connfd_, start);
}

void Connection::setTcpKeepAlive(bool start)
{
	util::setSocketKeepAlive(connfd_, start);
}

void Connection::handleRead(Timestamp receiveTime)
{
	int saveErrno;
	ssize_t n = inputBuffer_.readFd(connfd_, &saveErrno);
	LOG_INFO << "helloworld helloworld";
	if(n > 0){
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	}
	else if(n == 0){
		handleClose();
	}
	else{
		errno = saveErrno;
		handleError();
	}
}

void Connection::handleWrite()
{
	loop_->assertInLoopThread();
	if(channel_->isWriting()){
		ssize_t n = ::write(connfd_, outputBuffer_.peek(), outputBuffer_.readableBytes());
		if(n > 0){
			outputBuffer_.retrieve(n);
			if(outputBuffer_.readableBytes() == 0){
				channel_->disableWrite();
				if(state_ == kDisconnecting){
					shutdownInLoop();
				}
			}
		}
	}
}

void Connection::handleClose()
{
	assert(state_ == kConnected || state_ == kDisconnecting);
	channel_->disableAll();
	closeCallback_(shared_from_this());
}

void Connection::handleError()
{
	printf("this error is %d\n", util::getSockError(connfd_));
}
