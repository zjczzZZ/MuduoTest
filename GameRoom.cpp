#include "GameRoom.h"

GameRoom::GameRoom() {
    _playerNum = 0;
    rs = RoomStep::wait;
}

GameRoom::~GameRoom() {
}

void GameRoom::setRoomName(std::string str) {
    _roomName = str;
}

std::string GameRoom::getRoomName() {
    return _roomName;
}

bool GameRoom::AddRoom(Player* p) {
    if (_playerMap.find(p->_userID) != _playerMap.end()) {
        return false;
    } else {
        p->setRoomID(_rID);
        _playerMap[p->_userID] = p;
        _playerVector.emplace_back(p);
        return true;
    }
}

bool GameRoom::UserIsExistRoomID(uint64_t uID) {
    if (_playerMap.find(uID) == _playerMap.end()) {
        return false;
    } else {
        return true;
    }
}

void GameRoom::LeaveRoom(uint64_t uID) {
    if (_playerMap.find(uID) != _playerMap.end()) {
        (*_playerMap.find(uID)).second->setRoomID(-1);
        _playerMap.erase(_playerMap.find(uID));

        for (auto it = _playerVector.begin(); it != _playerVector.end(); it++) {
            if ((*it)->_userID == uID) {
                _playerVector.erase(it);
                return;
            }
        }
    }
}

std::vector<Player*>& GameRoom::getAllPlayer() {
    return _playerVector;
}
