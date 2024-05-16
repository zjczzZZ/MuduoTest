#pragma once

#include <muduo/net/TcpConnection.h>

#include <functional>
#include <mutex>
#include <unordered_map>

#include "MySQL.hpp"
#include "Redis.hpp"
#include "RoomManager.h"
#include "User.hpp"
#include "UserManager.h"
#include "UserModel.hpp"
#include "json.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

using json = nlohmann::json;
using MsgHandler = function<void(const TcpConnectionPtr& conn, json& js, Timestamp time)>;

class HandleService {
public:
    HandleService();

    ~HandleService();

    // 根据消息类型，返回相应处理函数
    MsgHandler GetHandler(int msgType);

    // 注册
    void Regist(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 登录
    void Login(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 退出
    void LoginOut(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 创建房间
    void CreateRoom(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 加入房间
    void AddRoom(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 离开房间
    void LeaveRoom(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 查询房间内玩家列表
    void QueryPlayerInRoomList(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 开始游戏
    void StartGame(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 查询房间列表
    void QueryRoomList(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 处理客户端异常退出
    void ClientCLoseException(const TcpConnectionPtr& conn);

    // 心跳包
    void KeepLive();

    // 服务器异常，业务重置
    void Reset();

private:
    unordered_map<int, MsgHandler> _msg_handle_map;

    unordered_map<int, TcpConnectionPtr> _user_connection_map;

    mutex _conn_mutex;

    Redis _redis;

    UserModel _user_model;

    RoomManager* _room_manager;

    UserManager* _user_manager;
};