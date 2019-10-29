 /// @file    server.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-21 19:41:36
 
#include "server.h"
#include "util.h"
#include "eventloop.h"
#include "connection.h"
#include "base/logging.h"
#include <assert.h>
using namespace std::placeholders;

Server::Server(EventLoop *loop, const InetAddress &listenAddr)
: loop_(loop)
, name_(listenAddr.toHostPort())
, acceptor_(new Acceptor(loop, listenAddr))
, threadpool_(new EventLoopThreadPool(loop))
, started_(false)
, nextConnId_(1)
{
	acceptor_->setConnectionCallback(std::bind(&Server::newConnection, this, _1, _2));
}

Server::~Server()
{}

void Server::start()
{
	if(!started_){
		started_ = true;
		threadpool_->start();
	}
	if(!acceptor_->listenning()){
		loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
	}
}

void Server::setThreadNum(int threadNums)
{
	threadpool_->setThreadNum(threadNums);
}

void Server::newConnection(int connfd, const InetAddress &clientAddr)
{
	loop_->assertInLoopThread();
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "#%d", nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	LOG_INFO << "newConnection [" << connName << "] " << "from [" << clientAddr.toHostPort() << "]";
	InetAddress localAddr(getLocalAddr(connfd));

	EventLoop *ioLoop = threadpool_->getNextLoop();
	TcpConnectionPtr conn(new Connection(ioLoop, connName, connfd, localAddr, clientAddr)); //注意Connection对象的生命周期，
	//该函数结束后Connection对象引用计数减1。由于将该conn对象放入了map中，因此引用计数加1。当断开连接将Connection对象从map中erase掉后，
	//该Connection对象的生命周期就岌岌可危了。注意理解...
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback(std::bind(&Server::removeConnection, this, _1)); 
	ioLoop->runInLoop(std::bind(&Connection::connectionEstablished, conn)); //将此函数放到io线程中调用
}

void Server::removeConnection(const TcpConnectionPtr &conn)
{
	loop_->runInLoop(std::bind(&Server::removeConnectionInLoop, this, conn));
}

void Server::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
	loop_->assertInLoopThread();
	connections_.erase(conn->name());
	EventLoop *ioLoop = conn->getLoop();
	ioLoop->queueInLoop(std::bind(&Connection::connectionDestroyed, conn));
}
