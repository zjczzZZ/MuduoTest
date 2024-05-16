#include "CentralServer.h"

#include <iostream>

#include "json.hpp"
using json = nlohmann::json;
using namespace std::placeholders;
using std::bind;

CentralServer::CentralServer(EventLoop* loop, const InetAddress& listenAddr, int idleSeconds)
    : _server(loop, listenAddr, "CentralServer"),
      _loop(loop),
      _connectionBuckets(idleSeconds) {
    // 注册连接回调函数
    _server.setConnectionCallback(bind(&CentralServer::onConnection, this, _1));

    // 注册消息回调函数
    _server.setMessageCallback(bind(&CentralServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);

    // 绑定定时器函数及步长
    _loop->runEvery(30.0, std::bind(&CentralServer::onTimer, this));

    _connectionBuckets.resize(idleSeconds);

    dumpConnectionBuckets();
}

void CentralServer::start() {
    _server.start();
}

void CentralServer::onConnection(const TcpConnectionPtr& conn) {
    LOG_INFO << "CentralServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    if (!conn->connected()) {
        assert(conn->getContext().has_value());
        WeakEntryPtr weakEntry(std::any_cast<WeakEntryPtr>(conn->getContext()));
        LOG_DEBUG << "Entry use_count = " << weakEntry.use_count();

        _handleService.ClientCLoseException(conn);
        conn->shutdown();
    } else {
        EntryPtr entry(new Entry(conn));
        _connectionBuckets.back().insert(entry);
        dumpConnectionBuckets();
        WeakEntryPtr weakEntry(entry);
        conn->setContext(weakEntry);
    }
}

void CentralServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
             << "data received at " << time.toString();

    LOG_INFO << msg;

    json js = json::parse(msg);

    auto handlerFunc = _handleService.GetHandler(js["msgid"].get<int>());

    handlerFunc(conn, js, time);

    assert(conn->getContext().has_value());
    WeakEntryPtr weakEntry(std::any_cast<WeakEntryPtr>(conn->getContext()));
    EntryPtr entry(weakEntry.lock());
    if (entry) {
        _connectionBuckets.back().insert(entry);
        dumpConnectionBuckets();
    }
}

void CentralServer::onTimer() {
    _connectionBuckets.push_back(BucketEntryPtr());
    dumpConnectionBuckets();
}

void CentralServer::dumpConnectionBuckets() const {
    LOG_INFO << "size= " << _connectionBuckets.size();
    int idx = 0;
    for (auto bucketIt = _connectionBuckets.begin(); bucketIt != _connectionBuckets.end(); ++bucketIt, ++idx) {
        const BucketEntryPtr& bucket = *bucketIt;
        cout << "[ " << idx << " ] len = " << bucket.size() << endl;

        for (const auto& it : bucket) {
            bool connectionTimeOut = it->_weakConn.expired();
            cout << "id " << get_pointer(it) << " " << it.use_count() << (connectionTimeOut ? "Dead" : "Alive") << endl;
        }
    }
}
