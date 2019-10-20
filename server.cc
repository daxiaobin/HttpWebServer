 /// @file    server.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-21 19:41:36
 
#include "server.h"
#include "util.h"
#include "eventloop.h"
#include "connection.h"
#include <assert.h>
using std::placeholders::_1;

Server::Server(EventLoop *loop, unsigned short port)
: loop_(loop)
, name_(std::string())
, acceptor_(new Acceptor(loop, port))
, threadpool_(new EventLoopThreadPool(loop))
, started_(false)
, nextConnId_(1)
{
	acceptor_->setConnectionCallback(std::bind(&Server::newConnection, this, _1));
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

void Server::newConnection(int connfd)
{
	loop_->assertInLoopThread();
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "#%d", nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

//	TcpConnectionPtr conn = std::make_shared<Connection>(loop_, connName, connfd);
	EventLoop *ioLoop = threadpool_->getNextLoop();
	TcpConnectionPtr conn(new Connection(ioLoop, connName, connfd));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback(std::bind(&Server::removeConnection, this, _1));
//	conn->connectionEstablished();
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
