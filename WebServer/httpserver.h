 /// @file    httpserver.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-22 15:33:33
 
#ifndef HTTPSERVER_H__
#define HTTPSERVER_H__


#include "httprequest.h"
#include "httpresponse.h"
#include "server.h"
using namespace std::placeholders;

class HttpServer : Noncopyable{
public:
	HttpServer(EventLoop *loop, unsigned short port)
	: server_(loop, port)
	{
		server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
		server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
	}

	void start();
	void setThreadNum(int threadNum) { server_.setThreadNum(threadNum); }
	EventLoop* getEventLoop() const { return server_.getEventLoop(); }
private:
	void onConnection(const Server::TcpConnectionPtr &conn);
	void onMessage(const Server::TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime);
	void onResponse(const Server::TcpConnectionPtr &conn,HttpRequest &reqe);

	static void setResponseContext(HttpRequest &request, HttpResponse &response);
private:
	Server server_;
};


#endif
