#include "RoomManager.h"

uint64_t RoomManager::rID = 0;

RoomManager::RoomManager() {
}

RoomManager::~RoomManager() {
    auto it = _RoomHashTable.begin();
    while (it != _RoomHashTable.end()) {
        if ((*it).second) {
            delete (*it).second;
        }
        it++;
    }
    _RoomList.clear();
}

GameRoom* RoomManager::CreateRoom(std::string rname) {
    GameRoom* gr = new GameRoom();
    gr->setRoomName(rname);
    std::lock_guard<std::mutex> lock(_mutex);
    while (_RoomHashTable.find(rID) != _RoomHashTable.end()) {
        rID++;
    }
    _RoomHashTable[rID] = gr;
    gr->setRoomID(rID);
    _RoomList[rID] = rname;
    return gr;
}

void RoomManager::DeleteRoom(uint64_t rid) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _RoomHashTable.find(rid);
    if (it != _RoomHashTable.end()) {
        delete (*it).second;
        _RoomHashTable.erase(it);
    }
    auto it2 = _RoomList.find(rid);
    if (it2 != _RoomList.end()) {
        _RoomList.erase(it2);
    }
}

bool RoomManager::ExistRoom(uint64_t rid) {
    return _RoomHashTable.find(rid) != _RoomHashTable.end();
}

GameRoom* RoomManager::GetRoom(uint64_t rid) {
    if (_RoomHashTable.find(rid) != _RoomHashTable.end()) {
        return _RoomHashTable[rid];
    } else {
        return nullptr;
    }
}

std::unordered_map<uint64_t, std::string>& RoomManager::GetRoomList() {
    return _RoomList;
}

void RoomManager::update() {
    std::lock_guard<std::mutex> lg(_mutex);
    auto it = _RoomHashTable.begin();
    while (it != _RoomHashTable.end()) {
        if ((it->second)->getPlayerNum() == 0) {
            if (_RoomList.find(it->second->getRoomID()) != _RoomList.end()) {
                _RoomList.erase(_RoomList.find((it->second)->getRoomID()));
            }

            it = _RoomHashTable.erase(it);
        } else
            it++;
    }
}
