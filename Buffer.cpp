//
// Created by miraclemilk on 2021/12/29.
//

#include <string>
#include "Buffer.h"
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>

Buffer::Buffer()
    :buffer(1024),
    readerIndex(0),
    writerIndex(0)
{
    assert(readableBytes()==0);
    assert(writeableBytes()==0);
}

size_t Buffer::writeableBytes() const
{
    return this->writerIndex - this->readerIndex;
}

size_t Buffer::readableBytes() const
{
    return this->buffer.size() - this->writerIndex;
}

size_t Buffer::prependableBytes() const
{
    return this->readerIndex;
}

const char* Buffer::peek() const
{
    return _begin() + this->readerIndex;
}

void Buffer::retrieve(size_t len)
{
    assert(len <= readableBytes());
    readerIndex += len;
}

void Buffer::retrieveUntil(const char *end)
{
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(end -peek());
}

void Buffer::retrieveAll()
{
    this->readerIndex = 0;
    this->writerIndex = 0;
}

std::string Buffer::retrieveString()
{
    std::string str(peek(),readableBytes());
    retrieveAll();
    return str;
}

void Buffer::append(const std::string &str)
{
    append(str.data(),str.length());
}

void Buffer::append(const char *data, size_t len)
{
    ensureWriteableBytes(len);
    std::copy(data,data+len,beginWrite());
    hasWritten(len);
}

void Buffer::append(const void *data, size_t len)
{
    append(static_cast<const char*>(data),len);
}

void Buffer::append(const Buffer &buffer)
{
    append(buffer.peek(),buffer.readableBytes());
}

void Buffer::ensureWriteableBytes(size_t len)
{
    if (writeableBytes() < len)
    {
        makeSpace(len);
    }
    assert(writeableBytes() >= len);
}

char* Buffer::beginWrite()
{
    return _begin() + this->writerIndex;
}

const char* Buffer::beginWrite() const
{
    return _begin() + this->writerIndex;
}

void Buffer::hasWritten(size_t len)
{
    this->writerIndex += len;
}

ssize_t Buffer::readFD(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writeable = writeableBytes();
    vec[0].iov_base = _begin() + this->writerIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const ssize_t n = ::readv(fd,vec,2);
    if(n<0)
    {
        std::cout<<"[Buffer::readerFd]fd = "<<fd<<" readv: "<<strerror(errno)<<std::endl;
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n)<= writeable)
    {
        this->writerIndex +=n;
    }
    else
    {
        this->writerIndex = this->buffer.size();
        append(extrabuf,n-writeable);
    }
    return n;
}

ssize_t Buffer::writeFD(int fd, int *savedErrno)
{
    size_t nLeft = readableBytes();
    char* bufPtr = _begin() + this->readerIndex;
    ssize_t n;
    if ((n=::write(fd,bufPtr,nLeft)) <= 0)
    {
        if (n<0 && n== EINTR)
            return 0;
        else
        {
            std::cout<<"[Buffer::writeFd]fd = "<<fd<<" write: "<<strerror(errno)<<std::endl;
            *savedErrno = errno;
            return -1;
        }
    }
    else
    {
        this->readerIndex += n;
        return n;
    }
}

const char* Buffer::findCRLF() const
{
    const char CRLF[] = "\r\n";
    const char* crlf = std::search(peek(),beginWrite(),CRLF,CRLF+2);
    return crlf = beginWrite() ? nullptr : crlf;
}

const char* Buffer::findCRLF(const char *start) const
{
    assert(peek() <= start);
    assert(start <= beginWrite());
    const char CRLF[] = "\r\n";
    const char* crlf = std::search(peek(),beginWrite(),CRLF,CRLF+2);
    return crlf = beginWrite() ? nullptr : crlf;
}

char* Buffer::_begin()
{
    return &*this->buffer.begin();
}

const char* Buffer::_begin() const
{
    return &*this->buffer.begin();
}

void Buffer::makeSpace(size_t len)
{
    if (writeableBytes() + prependableBytes() < len)
    {
        this->buffer.resize(writerIndex + len);
    }
    else
    {
        size_t readable = readableBytes();
        std::copy(_begin()+this->readerIndex,_begin()+this->writerIndex,_begin());
        this->readerIndex = 0;
        this->writerIndex = this->readerIndex + readable;
        assert(readable == readableBytes());
    }
}