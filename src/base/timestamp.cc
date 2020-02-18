 /// @file    timestamp.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-10-14 10:59:29
 
#include "timestamp.h"
#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif


Timestamp::Timestamp()
: microsecondsSinceEpoch_(0)
{}

Timestamp::Timestamp(uint64_t microsecondsSinceEpoch)
: microsecondsSinceEpoch_(microsecondsSinceEpoch)
{}

string Timestamp::toString() const
{
	char buf[32] = {0};
	int64_t seconds = microsecondsSinceEpoch_ / kMicrosecondsPerSecond;
	int64_t microseconds = microsecondsSinceEpoch_ % kMicrosecondsPerSecond;
	snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

string Timestamp::toFormattedString(bool showMicroseconds) const
{
	char buf[64] = {0};
	time_t seconds = static_cast<time_t>(microsecondsSinceEpoch_ / kMicrosecondsPerSecond);
	struct tm tm_time;
	gmtime_r(&seconds, &tm_time);

	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(microsecondsSinceEpoch_ % kMicrosecondsPerSecond);
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
				microseconds);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;
	return Timestamp(seconds * kMicrosecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::addTime(Timestamp timestamp, double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds * kMicrosecondsPerSecond);
	return Timestamp(timestamp.microsecondsSinceEpoch() + delta);
}
