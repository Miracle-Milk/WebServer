//
// Created by miraclemilk on 2021/12/29.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H
#include <vector>

class Buffer {
public:
    Buffer();
    ~Buffer(){}
    size_t readableBytes() const;       //可读字节数
    size_t writeableBytes() const;      //可写字节数
    size_t prependableBytes() const;    //reader index前面的空闲缓冲区大小
    const char* peek() const;           //第一个可读位置
    void retrieve(size_t len);          //取出len个字节
    void retrieveUntil(const char* end);//取出数据直到end
    void retrieveAll();                 //取出buffer中的所有数据
    std::string retrieveString();       //以string类型取出所有数据
    void append(const std::string& str);//插入string数据
    void append(const char* data,size_t len);       //插入char* 数据
    void append(const void* data,size_t len);       //插入void* 数据
    void append(const Buffer& buffer);              //插入其他缓冲区的数据
    void ensureWriteableBytes(size_t len);          //确保缓冲区有足够的空间
    char* beginWrite();                             //可写char指针
    const char* beginWrite() const;                 //
    void hasWritten() const;                        //写完数据后移动writeindex指针
    ssize_t readFD(int fd,int* savedErrno);         //从套接字读到缓冲区
    ssize_t writeFD(int fd,int* savedErrno);        //从缓冲区写入套接字
    const char* findCRLF() const;                   //
    const char* findCRLF(const char* start) const;  //
private:
    char* begin();                      //返回缓冲区头指针
    const char* begin() const;
    void makeSpace(size_t len);         //确保缓冲区有足够空间
private:
    std::vector<char> buffer;
    size_t readerIndex;
    size_t writerIndex;
};


#endif //WEBSERVER_BUFFER_H
