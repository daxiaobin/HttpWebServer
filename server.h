 /// @file    server.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-19 10:36:54
 
#ifndef SERVER_H__
#define SERVER_H__

//Server类的作用是管理由accept返回的新连接

#include "base/noncopyable.h"
#include "base/timestamp.h"
#include "eventloopthreadpool.h"
#include "acceptor.h"
#include <memory>
#include <functional>
#include <map>
#include <string>

class EventLoop;
class Connection;
class Buffer;

class Server : Noncopyable
{
public:
	typedef std::shared_ptr<Connection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback; 
	typedef std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)> MessageCallback;
			
	Server(EventLoop *loop, unsigned short port);
	~Server();

	void start();
	void setThreadNum(int threadNum);
	EventLoop* getEventLoop() const { return loop_; }

	void setConnectionCallback(const ConnectionCallback &cb)
	{ connectionCallback_ = cb; }
	void setMessageCallback(const MessageCallback &cb)
	{ messageCallback_ = cb; }
private:
	void newConnection(int connfd);
	void removeConnection(const TcpConnectionPtr &conn);
	void removeConnectionInLoop(const TcpConnectionPtr &conn);
	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
private:
	EventLoop *loop_; // for acceptor
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_;
	std::unique_ptr<EventLoopThreadPool> threadpool_;
	bool started_;
	int nextConnId_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	ConnectionMap connections_;
};



#endif
