 /// @file    connection.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-21 20:40:50
 
#ifndef CONNECTION_H__
#define CONNECTION_H__


#include "base/noncopyable.h"
#include "base/timestamp.h"
#include "buffer.h"
#include "inetaddress.h"
#include <list>
#include <memory>
#include <string>

class EventLoop;
class Channel;
class Connection : public std::enable_shared_from_this<Connection>, Noncopyable
{
public:
	typedef std::shared_ptr<Connection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback; 
	typedef std::function<void(const TcpConnectionPtr &, Buffer *buf, Timestamp)> MessageCallback;
	typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;

	Connection(EventLoop *loop, const std::string &name, int connfd, const InetAddress &localAddr, const InetAddress &peerAddr);
	~Connection();

	EventLoop* getLoop() const { return loop_; }
	const std::string& name() const { return name_; }
	bool connected() const { return state_ == kConnected; }
	const InetAddress& localAddr() const { return localaddr_; }
	const InetAddress& peerAddr() const { return peerAddr_; }

	void setConnectionCallback(const ConnectionCallback &cb)
	{ connectionCallback_ = cb; }
	void setMessageCallback(const MessageCallback &cb)
	{ messageCallback_ = cb; }
	void setCloseCallback(const CloseCallback &cb)
	{ closeCallback_ = cb; }
	
	void connectionEstablished();
	void connectionDestroyed();

	void send(const std::string &message);
	void shutdown();

	void setTcpNoDelay(bool start);
	void setTcpKeepAlive(bool start);

	//for KeepAlive
	struct Node
	{
		Timestamp lastReceiveTime;

		typedef std::weak_ptr<Connection> WeakConnectionPtr;
		typedef std::list<WeakConnectionPtr> WeakConnectionList;
		WeakConnectionList::iterator position; //该conn位于链表的位置，实现常数级链表查找。
	};
	Node& getNode() { return node_; }
	void setNode(const Node &node) { node_ = node; }
private:
	enum stateE{ kConnecting, kConnected, kDisconnected, kDisconnecting };
	void setState(stateE s) { state_ = s; }

	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();

	void sendInLoop(const std::string &message);
	void shutdownInLoop();
private:
	EventLoop *loop_;
	int connfd_;
	std::string name_;
	std::unique_ptr<Channel> channel_; //Connection对象销毁的时候也就是channel_释放内存的时候。
	stateE state_;
	InetAddress localaddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	CloseCallback closeCallback_;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
	//for KeepAlive
	Node node_;
};



#endif
