#pragma once
#include <muduo/base/noncopyable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>
class Player : muduo::noncopyable {
public:
    Player();
    ~Player();

public:
    void setUserName(std::string username) { _username = username; }
    void setPasswd(std::string passwd) { _passwd = passwd; }
    void setIP(std::string ip) { _ip = ip; }
    void setPort(uint16_t port) { _port = port; }

    std::string getUserName() const { return _username; }
    std::string getPassws() const { return _passwd; }
    std::string getIP() const { return _ip; }
    uint16_t getPort() const { return _port; }

public:
    void CreateRoom(uint64_t rID);
    void EntryRoom(uint64_t rID);
    void LeaveRoom(uint64_t rID);
    const uint64_t getRoomID();
    void setRoomID(uint64_t rID);
    bool IsOwner();

    uint64_t _userID;

private:
    std::string _username;
    std::string _passwd;

    std::string _ip;
    uint16_t _port;
    uint64_t _roomID;
    bool _owner;
};