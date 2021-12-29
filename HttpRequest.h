//
// Created by miraclemilk on 2021/12/29.
//

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H
#include <iostream>
#include <string>
#include <>

class HttpRequest {
public:
    enum HttpRequestParseState      //报文解析状态
    {
        ExpectRequestLine,
        ExpectHeadlers,
        ExpectBody,
        GotAll
    };

    enum Method                     //Http 方法
    {
        Invalid,Get,Post,Head,Put,Delete
    };

    enum Version                    //Http 版本
    {
        Unknow,HTTP10,HTTP11
    };

    HttpRequest(int fd);
    ~HttpRequest();

private:
    int fd;
    Buffer inBuff;
};


#endif //WEBSERVER_HTTPREQUEST_H
