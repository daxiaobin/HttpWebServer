 /// @file    httpserver.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-22 16:10:55
 
#include "httpserver.h"
#include "connection.h"
#include "base/logging.h"

void HttpServer::start()
{
	server_.start();
}

void HttpServer::onConnection(const Server::TcpConnectionPtr &conn)
{
	if(conn->connected()){
		LOG_INFO << "已建立连接";
	}
	else{
		LOG_INFO << "断开连接";
	}
}

void HttpServer::onMessage(const Server::TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
	HttpRequest request; //在这里定义HttpRequest对象是为了保证线程安全，可以不用锁实现线程安全。开始的想法是将HttpRequest对象定义为类的成员变量。
	int state = request.parseRequest(buf, receiveTime);
	if(kGotAll != state){
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}
	else{
		onResponse(conn, request);
	}
}

void HttpServer::onResponse(const Server::TcpConnectionPtr &conn, HttpRequest &reqe)
{
	const string isShortConnection = reqe.getHeaderFieldValue("Connection");
	bool close = (isShortConnection == "close" || reqe.getVersion() == kHttp10); //HTTP/1.O不支持长连接。
	HttpResponse response(close);
	setResponseContext(reqe, response); //设置response
	Buffer buf;
	response.appendToBuffer(&buf); //将response对象中的内容写入到buf中
	conn->send(buf.retrieveAsString());
	if(response.isShortConnection()){
		conn->shutdown();
	}
}

void HttpServer::setResponseContext(HttpRequest &request, HttpResponse &response) //这是HTTP响应到来的请求的逻辑处理函数
{
	if(request.getPath() == "/"){
		string path("/docs/index.html");
		request.setPath(path.data(), path.data() + path.size());
	}
	response.setStatusCode(k200OK);
	response.setReasonMessage("OK");
	response.setCloseConnection(false);
	response.setBody(request.getPath());
	response.setContentType("html");
}
