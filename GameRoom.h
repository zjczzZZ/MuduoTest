#pragma once
#include <muduo/base/noncopyable.h>
#include <unistd.h>

#include <unordered_map>
#include <vector>

#include "Player.h"

enum RoomStep {
    wait = 0,
    running,
    end,
};

class GameRoom : public muduo::noncopyable {
public:
    GameRoom();
    ~GameRoom();

public:
    const RoomStep& getRoomStatus() { return rs; }
    const uint8_t& getPlayerNum() { return _playerNum; }
    uint64_t getRoomID() { return _rID; }
    void setRoomID(uint64_t id) { _rID = id; }

    void setRoomName(std::string str);
    std::string getRoomName();

    bool AddRoom(Player* p);
    bool UserIsExistRoomID(uint64_t uID);
    void LeaveRoom(uint64_t uID);
    std::vector<Player*>& getAllPlayer();

private:
    uint64_t _rID;
    uint8_t _playerNum;
    std::string _roomName;
    RoomStep rs;
    std::unordered_map<uint64_t, Player*> _playerMap;
    std::vector<Player*> _playerVector;
};