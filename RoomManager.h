#pragma once

#include <muduo/base/noncopyable.h>

#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "GameRoom.h"
#include "Player.h"

class RoomManager : public muduo::noncopyable {
public:
    RoomManager();
    ~RoomManager();

public:
    GameRoom* CreateRoom(std::string rname);

    void DeleteRoom(uint64_t rid);

    bool ExistRoom(uint64_t rid);

    GameRoom* GetRoom(uint64_t rid);

    std::unordered_map<uint64_t, std::string>& GetRoomList();

    void update();

private:
    static uint64_t rID;

    std::unordered_map<uint64_t, GameRoom*> _RoomHashTable;

    std::unordered_map<uint64_t, std::string> _RoomList;

    std::mutex _mutex;
};