 /// @file    test.cc
 // @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-16 19:13:16

#include "httpserver.h"
#include "eventloop.h"
#include "base/logging.h"
 
/*#include "eventloop.h"
#include "channel.h"
#include "acceptor.h"
#include "buffer.h"
#include "base/timestamp.h"
#include <stdio.h>
#include <string.h>
#include <sys/timerfd.h>
#include "connection.h"
#include "server.h"

using namespace std::placeholders;

typedef std::shared_ptr<Connection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback; 
typedef std::function<void(const TcpConnectionPtr &, Buffer *buf, Timestamp)> MessageCallback;

class EchoServer
{
public:
	typedef std::weak_ptr<Connection> WeakConnectionPtr;
	typedef std::list<WeakConnectionPtr> WeakConnectionList;

	EchoServer(EventLoop *loop, const short port, int idleSeconds);
	void start() { server_.start(); }

private:
	void onConnection(const TcpConnectionPtr &conn);
	void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time);
	void onTime();
private:
	Server server_;
	int idleSeconds_;
	WeakConnectionList connectionList_;
};

EchoServer::EchoServer(EventLoop *loop, const short port, int idleSeconds)
: server_(loop, port)
, idleSeconds_(idleSeconds)
{
	server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
	server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
	loop->runEvery(1.0, std::bind(&EchoServer::onTime, this));
}

void EchoServer::onConnection(const TcpConnectionPtr &conn)
{
	if(conn->connected()){
		printf("yes\n");
		Connection::Node node;
		node.lastReceiveTime = Timestamp::now();
		connectionList_.push_back(conn);
		node.position = --connectionList_.end();
		conn->setNode(node);
	}
	else{
		printf("no\n");
		connectionList_.erase(conn->getNode().position);
	}
}

void EchoServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
	string msg(buffer->retrieveAsString());
	conn->send(msg);

	Connection::Node &node = conn->getNode();
	node.lastReceiveTime = time;
	connectionList_.splice(connectionList_.end(), connectionList_, node.position);
	assert(node.position == --connectionList_.end());
}

void EchoServer::onTime()
{
	Timestamp now = Timestamp::now();
	for(auto it = connectionList_.begin(); it != connectionList_.end(); ++it){
		TcpConnectionPtr conn = it->lock();
		if(conn){
			int diff = (now.microsecondsSinceEpoch() - conn->getNode().lastReceiveTime.microsecondsSinceEpoch()) / Timestamp::kMicrosecondsPerSecond;
			printf("diff = %d\n", diff);
			if(diff > idleSeconds_){
				if(conn->connected())
					conn->shutdown();
			}
			else{
				break;
			}
		}
	}
}

int main()
{
	EventLoop loop;
	int idleSeconds = 10;
	EchoServer server(&loop, 9981, idleSeconds);
	server.start();
	loop.loop();
}*/

int main(int argc, char *argv[])
{
	int threadNum = 0;
	if(argc > 1){
		threadNum = atoi(argv[1]);
		Logger::setLogLevel(Logger::WARN);
	}

	EventLoop loop;
	HttpServer httpServer(&loop, 80);
	httpServer.setThreadNum(threadNum);
	httpServer.start();
	loop.loop();

	return 0;
}
