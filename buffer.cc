 /// @file    buffer.cc
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-24 20:09:31
 
#include "buffer.h"
#include <errno.h>
#include <sys/uio.h>

ssize_t Buffer::readFd(int fd, int *saveErrno)
{
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writeable = writeableBytes();
	vec[0].iov_base = buffer_.data() + writeIndex_;
	vec[0].iov_len = writeable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);
	const ssize_t n = readv(fd, vec, 2);
	if(n < 0){
		*saveErrno = errno;
	}
	else if(static_cast<size_t>(n) <= writeable){
		writeIndex_ += n;
	}
	else{
		writeIndex_ = buffer_.size();
		append(extrabuf, n - writeable);
	}
	return n;
}
