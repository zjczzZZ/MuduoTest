#include "CentralServer.h"
#include "json.hpp"

using json = nlohmann::json;

int main() {
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(8888);
    CentralServer server(&loop, listenAddr, 2);
    server.start();
    loop.loop();

    json tmp1;
}
