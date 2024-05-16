#pragma once
#include <unordered_map>
#include <vector>

#include "Player.h"

class UserManager {
private:
public:
    UserManager();
    ~UserManager();

    void UserLogin(Player* p);
    void UserLogout(uint64_t id);
    bool UserOnline(uint64_t id);
    Player* getPlayer(uint64_t id);

private:
    std::unordered_map<uint64_t, Player*> _player_map;
};
