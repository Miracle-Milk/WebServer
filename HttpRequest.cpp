//
// Created by miraclemilk on 2021/12/29.
//

#include "HttpRequest.h"

#include <iostream>
#include <unistd.h>
#include <cassert>

HttpRequest::HttpRequest(int fd)
    :fd(fd),
    working(false),
    timer(nullptr),
    state(ExpectRequestLine),
    method(Invalid),
    version(Unknow)
{
    assert(fd >= 0);
}

HttpRequest::~HttpRequest()
{
    close(this->fd);
}

int HttpRequest::read(int *savedError)
{
    int ret = this->inBuff.readFD(this->fd,savedError);
    return ret;
}

int HttpRequest::write(int *savedError)
{
    int ret = this->outBuff.writeFD(this->fd,savedError);
    return ret;
}

bool HttpRequest::parseRequest()
{
    bool ok = true;
    bool hasMore = true;

    while(hasMore)
    {
        //
    }
    return ok;
}

bool HttpRequest::parseRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start,end,' ');
    if(space != end && setMethod(start,space))
    {
        start = space + 1;
        space = std::find(start,end,' ');
        if(space != end)
        {
            const char* question = std::find(start,space,'?');
            if (question != space)
            {
                setPath(start,question);
                setQuery(question,start);
            } else
            {
                setPath(start,space);
            }
            start = space + 1;
            succeed = end -start == 8 && std::equal(start,end-1,"HTTP/1.");
            if(succeed)
            {
                if(*(end-1) == '1')
                    setVersion(HTTP11);
                else if(*(end-1) == '0')
                    setVersion(HTTP10);
                else
                    succeed = false;
            }
        }
    }
    return succeed;
}

bool HttpRequest::setMethod(const char *begin, const char *end)
{
    std::string m(begin,end);
    if(m == "GET")
        this->method = Get;
    else if(m == "POST")
        this->method = Post;
    else if(m == "HEAD")
        this->method = Head;
    else if(m == "PUT")
        this->method = Put;
    else if(m == "DELETE")
        this->method = Delete;
    else
        this->method = Invalid;

    return this->method != Invalid;
}

void HttpRequest::addHeader(const char *start, const char *colon, const char *end)
{
    std::string filed(start,colon);
    ++colon;
    while(colon < end && *colon == ' ')
        ++colon;
    std::string value(colon,end);
    while(!value.empty() && value[value.size() - 1] == ' ')
        value.resize(value.size() - 1);

    this->headers[filed] = value;
}

std::string HttpRequest::getMethed() const
{
    std::string res;
    if(this->method == Get)
        res = "GET";
    else if(this->method == Post)
        res = "POST";
    else if(this->method == Head)
        res = "HEAD";
    else if(this->method == Put)
        res = "Put";
    else if(this->method == Delete)
        res = "DELETE";

    return res;
}

std::string HttpRequest::getHeader(const std::string filed) const
{
    std::string res;
    auto itr = this->headers.find(filed);
    if(itr != this->headers.end())
        res = itr->second;
    return res;
}

bool HttpRequest::keepAlive() const
{
    std::string connection = getHeader("Connection");
    bool res = connection == "Keep-Alive" ||(this->version == HTTP11 && connection != "close");
    return res;
}

void HttpRequest::resetParse()
{
    this->state = ExpectRequestLine;
    this->method = Invalid;
    this->version = Unknow;
    this->path = "";
    this->query = "";
    this->headers.clear();
}