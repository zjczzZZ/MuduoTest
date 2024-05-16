#include "UserManager.h"

UserManager::UserManager() {
    _player_map.clear();
}

UserManager::~UserManager() {
    auto it = _player_map.begin();

    while (it != _player_map.end()) {
        if ((*it).second) {
            delete (*it).second;
        }
    }
}

void UserManager::UserLogin(Player* p) {
    _player_map[p->_userID] = p;
}

void UserManager::UserLogout(uint64_t id) {
    auto it = _player_map.find(id);

    if (it != _player_map.end()) {
        if ((*it).second) {
            delete (*it).second;
        }
        _player_map.erase(it);
    }
}

bool UserManager::UserOnline(uint64_t id) {
    auto it = _player_map.find(id);

    if (it != _player_map.end()) {
        return true;
    } else {
        return false;
    }
}

Player* UserManager::getPlayer(uint64_t id) {
    auto it = _player_map.find(id);
    if (it == _player_map.end()) {
        return nullptr;
    } else {
        return (*it).second;
    }
}