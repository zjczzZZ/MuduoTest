#pragma once
#include <string>

enum CMsgType {
    LOGIN_MSG = 1,     // 登录消息，绑定login
    LOGIN_MSG_ACK,     // 登录响应消息
    REG_MSG,           // 注册消息，绑定regist
    REG_MSG_ACK,       // 注册响应消息
    LOGINOUT_MSG,      // 注销消息
    LOGINOUT_MSG_ACK,  // 注销消息响应

    CREATE_ROOM_MSG,   // 创建房间
    ADD_ROOM_MSG,      // 加入房间
    ADD_ROOM_MSG_ACK,  // 加入房间响应
    LEAVE_ROOM_MSG,    // 离开房间

    QUERY_ROOM_LIST_MSG,      // 查询房间列表
    QUERY_ROOM_LIST_MSG_ACK,  // 房间列表响应

    QUERY_ROOM_USER_LIST_MSG,      // 查询房间内玩家列表
    QUERY_ROOM_USER_LIST_MSG_ACK,  // 房间内玩家列表响应

    START_GAME,      // 开始游戏
    START_GAME_ACK,  // 开始游戏响应

    KeepLive,  // 心跳包

};

struct MsgHeaher {
    CMsgType mt;
};

struct Login {
    char username[16];
    char passwd[16];
};

enum RoomComType {
    Create = 0,
    Entry,
    Leave,
    Start,
};

struct RoomCommend {
    RoomComType rct;
    uint16_t roomID;
    uint8_t PlayerMaxNum;
};
