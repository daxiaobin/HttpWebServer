 /// @file    buffer.h
 /// @author  daxiaobing(248918860@qq.com)
 /// @date    2019-09-24 17:45:10
 
#ifndef BUFFER_H__
#define BUFFER_H__

#include <vector>
#include <unistd.h>
#include <string>
#include <assert.h>
#include <algorithm>

class Buffer
{
public:
	static const size_t kCheapPrepend = 8;
	static const size_t kInitialSize = 1024;

	Buffer()
	: buffer_(kCheapPrepend + kInitialSize)
	, readIndex_(kCheapPrepend)
	, writeIndex_(kCheapPrepend)
	{}

	void swap(Buffer &rhs){
		buffer_.swap(rhs.buffer_);
		std::swap(readIndex_, rhs.readIndex_);
		std::swap(writeIndex_, rhs.writeIndex_);
	}
	const char* peek() const{
		return buffer_.data() + readIndex_;
	}

	size_t readableBytes() const{
		return writeIndex_ - readIndex_;
	}
	size_t writeableBytes() const{
		return buffer_.size() - writeIndex_;
	}
	size_t prependableBytes() const{
		return readIndex_;
	}

	void retrieve(size_t len){
		assert(len <= readableBytes());
		readIndex_ += len;
	}
	void retrieveAll(){
		readIndex_ = kCheapPrepend;
		writeIndex_ = kCheapPrepend;
	}
	void retrieveUntil(const char *end){
		assert(peek() <= end);
		assert(end <= buffer_.data() + writeIndex_);
		retrieve(end - peek());
	}
	std::string retrieveAsString(){
		std::string str(peek(), readableBytes());
		retrieveAll();
		return str;
	}

	void append(const std::string& str){
		append(str.data(), str.length());
	}
	void append(const void *data, size_t len){
		append(static_cast<const char*>(data), len);
	}
	void append(const char *data, size_t len){
		ensureWriteableBytes(len);
		std::copy(data, data + len, buffer_.data() + writeIndex_);
		writeIndex_ += len;
	}

	void ensureWriteableBytes(size_t len){
		if(writeableBytes() < len){
			makeSpace(len);
		}
		assert(writeableBytes() >= len);
	}

	void prepend(const void *data, size_t len){
		assert(prependableBytes() >= len);
		readIndex_ -= len;
		const char *d = static_cast<const char*>(data);
		std::copy(d, d + len, buffer_.data() + readIndex_);
	}
	
	void shrink(size_t reserve){
		std::vector<char> buf(kCheapPrepend + readableBytes() + reserve);
		std::copy(peek(), peek() + readableBytes(), buf.data() + kCheapPrepend);
		buf.swap(buffer_);
	}


	ssize_t readFd(int fd, int *saveErrno);
private:
	void makeSpace(size_t len){
		if(writeableBytes() + prependableBytes() < len + kCheapPrepend){
			buffer_.resize(writeIndex_ + len);
		}
		else{
			size_t readable = readableBytes();
			std::copy(buffer_.data() + readIndex_, buffer_.data() + writeIndex_, buffer_.data() + kCheapPrepend);
			readIndex_ = kCheapPrepend;
			writeIndex_ = readIndex_ + readable;
		}
	}
private:
	std::vector<char> buffer_;
	size_t readIndex_;
	size_t writeIndex_;
};


#endif
