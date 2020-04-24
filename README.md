# 并发网络库
[![Build Status](https://travis-ci.com/daxiaobin/HttpWebServer.svg?branch=master)](https://travis-ci.com/daxiaobin/HttpWebServer)
![](https://img.shields.io/badge/language-c++-blue.svg)
![](https://img.shields.io/badge/license-MIT-red.svg)

## 简介

本项目使用C++11编写，操作系统使用的是Ubuntu 16.04，使用g++6.5.0版本进行编译。项目实现了一个简单HTTP1.1服务器，可处理get、head请求，重点在于底层高并发的多线程网络库。网络库的模型如下![网络库模型](https://github.com/daxiaobin/HttpWebServer/blob/master/datum/model.png)在开启多线程后，主线程只负责处理新连接的到来，后续交给线程池中的I/O线程处理，采用Round-Robin方式进行分配。整体遵循One loop per thread + ThreadPool 思想，即线程池中各个线程最大限度的独立运行，减少对象在线程间的共享，从而很好的保障了线程安全。

## 使用

1. 进入src目录
2. make
3. ./test [thread-numbers]

## 技术点

- 使用One loop per thread + ThreadPool 的并发模型，在满足高并发的基础上还很好的保证了线程安全问题，因为尽量减少了类成员函数的跨线程调用。
- 使用Epoll水平触发，搭配非阻塞IO。使用Reactor模式。
- 在程序启动初期就创建线程池，避免后续频繁的创建和销毁线程，并且充分利用多核的CPU。
- 使用gettimeofday得到时间，使用timerfd计时，好处是由于timerfd是基于文件描述符的，可以融入IO多路复用，从而统一事件源。
- 设计了应用层buffer，使得用户只需关注上层的逻辑处理，不用关心数据发送和接收的底层过程，这些都交给网络库处理。
- 实现了双缓冲异步日志，核心思想是单独开一个日志线程负责收集日志消息，并写入日志文件，其他IO线程只负责往这个日志线程中写日志就可以。
- 使用eventfd来进行线程间的事件通知或唤醒线程。
- 实现优雅关闭连接，确保两端都能在连接关闭之前收到所有的对方数据。
- 使用智能指针防止内存泄漏以及管理对象生命周期，因为裸指针没办法直到其所指对象是否存在。

