//
// Created by miraclemilk on 2021/12/29.
//

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H
#include <iostream>
#include <string>
#include <map>

#include "Time.h"
#include "Buffer.h"

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

    int getFD() {return fd;}                                //返回文件描述符
    int read(int* savedError);                              //读数据
    int write(int* savedError);                             //写数据

    void appendOutBuffer(const Buffer& buf);                //
    int writeableBytes() {return outBuff.readableBytes();}  //

    void setTime(Time* time) {timer = time;}                //
    Time* getTime() {return timer;}                         //

    void setWorking() {working = true;}                     //
    void setNotWorking() {working = false;}                 //
    bool isWoring() const {return working;}                 //

    bool parseRequest();                                    //解析HTTP报文
    bool parseFinish() {state = GotAll;}                    //是否解析完一个报文
    void resetParse();                                      //重置解析状态
    std::string getPath() {return path;}                    //
    std::string getQuery() {return query;}                  //
    std::string getHeader(const std::string filed) const;   //
    std::string getMethed() const;                          //
    bool keepAlive() const;                                 //是否是长连接


private:
    //
    bool parseRequestLine(const char* begin,const char* end);
    //设置HTTP方法
    bool setMethod(const char* begin,const char* end);
    //设置URL路径
    void setPath(const char* begin,const char* end)
    {
        std::string subpath;
        subpath.assign(begin,end);
        if(subpath =="/")
            subpath = "/index.html";
        this->path = "../www" + subpath;
    }
    //设置URL参数
    void setQuery(const char* begin,const char* end)
    {this->query.assign(begin,end);}
    //设置HTTP版本
    void setVersion(Version version)
    { this->version = version;}
    //增加报文头
    void addHeader(const char* start,const char* colon,const char* end);

private:
    //网络通信
    int fd;             //文件描述符
    Buffer inBuff;      //读缓冲
    Buffer outBuff;     //写缓冲
    bool working;       //若正在工作，则不能被超时事件断开连接

    //计时器
    Time* timer;

    //报文解析
    HttpRequestParseState state;                //报文解析状态
    Method method;                              //HTTP方法
    Version version;                            //HTTP版本
    std::string path;                           //URL路径
    std::string query;                          //URL参数
    std::map<std::string,std::string> headers;  //报文头部
};


#endif //WEBSERVER_HTTPREQUEST_H
