 /// @file    timestamp.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-17 10:29:33
 
#ifndef TIMESTAMP_H__
#define TIMESTAMP_H__

#include <string>
#include <stdint.h>
using std::string;


class Timestamp
{
public:
	static const uint64_t kMicrosecondsPerSecond= 1000 * 1000;
public:
	Timestamp();
	explicit Timestamp(uint64_t microsecondsSinceEpoch);

	uint64_t microsecondsSinceEpoch() const { return microsecondsSinceEpoch_; }
	bool valid() const { return microsecondsSinceEpoch_ > 0; }

	string toString() const;
	string toFormattedString(bool showMicroseconds = true) const;

	static Timestamp now(); // get time of now
	static Timestamp invalid(){ // get an invalid time
		return Timestamp();
	} 
	static Timestamp addTime(Timestamp timestamp, double seconds);

	bool operator<(const Timestamp &rhs) const{ return microsecondsSinceEpoch_ < rhs.microsecondsSinceEpoch_; }
	bool operator==(const Timestamp &rhs) const { return microsecondsSinceEpoch_ == rhs.microsecondsSinceEpoch_; }
private:
	uint64_t microsecondsSinceEpoch_; //微妙，1秒=1000*1000微妙
};


#endif
