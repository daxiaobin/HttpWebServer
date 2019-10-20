 /// @file    acceptor.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-19 09:29:07
 
#include "acceptor.h"
#include "eventloop.h"
#include "util.h"
#include <arpa/inet.h>
#include <string.h>

Acceptor::Acceptor(EventLoop *loop, unsigned short port)
: listenning_(false)
, loop_(loop)
, port_(port)
, listenFd_(socket_bind_listen(port))
, channel_(loop_, listenFd_)
{
	channel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
	::close(listenFd_);
}

void Acceptor::listen()
{
	loop_->assertInLoopThread();
	listenning_ = true;
	channel_.enableReading();
}

void Acceptor::handleRead()
{
	loop_->assertInLoopThread();

	//FIXME 这些写在这里是没有意义的
	struct sockaddr_in client_addr;
	bzero(&client_addr, sizeof(client_addr));
	socklen_t len = sizeof(client_addr);

	int connfd = accept(listenFd_, (struct sockaddr*)&client_addr, &len);
	if(connfd >= 0){
		if(newConnectionCallback_){
			setSocketNonBlocking(connfd);
			newConnectionCallback_(connfd);
		}
		else{
			::close(connfd);
		}
	}
}
