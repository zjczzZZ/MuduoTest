#pragma once

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <any>
#include <boost/circular_buffer.hpp>
#include <functional>
#include <iostream>
#include <unordered_set>

#include "HandleService.h"
#include "data.h"

using namespace muduo;
using namespace muduo::net;
using std::string;

// 使用muduo开发回显服务器
class CentralServer {
public:
    CentralServer(EventLoop* loop, const InetAddress& listenAddr, int idleSeconds);

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);

    void onTimer();

    void dumpConnectionBuckets() const;

private:
    typedef std::weak_ptr<TcpConnection> WeakTcpConnectionPtr;

    // 用于将连接与时间轮关联
    // 超时后将析构，断开连接
    struct Entry : public copyable {
        explicit Entry(const WeakTcpConnectionPtr& weakConn)
            : _weakConn(weakConn) {
        }

        ~Entry() {
            TcpConnectionPtr conn = _weakConn.lock();
            if (conn) {
                conn->shutdown();
                std::cout << "dddddd" << std::endl;
            }
        }

        WeakTcpConnectionPtr _weakConn;
    };

    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    // 存entry指针的集合，同一时间触发的entry都存在同一集合的bucket中
    typedef std::unordered_set<EntryPtr> BucketEntryPtr;
    // 定义时间轮的环形队列
    typedef boost::circular_buffer<BucketEntryPtr> WeakConnectionList;

    // 时间轮的环形队列
    WeakConnectionList _connectionBuckets;

private:
    TcpServer _server;
    EventLoop* _loop;
    HandleService _handleService;
};