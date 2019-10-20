 /// @file    noncopyable.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-05 18:55:33
 
#ifndef NONCOPYABLE_H__
#define NONCOPYABLE_H__


class Noncopyable{
public:
	Noncopyable() = default;
	~Noncopyable(){}

	Noncopyable(const Noncopyable&) = delete;
	void operator=(const Noncopyable&) = delete;
};


#endif
