#include "HandleService.h"

#include <muduo/base/Logging.h>

#include <iostream>
#include <map>
#include <vector>

#include "data.h"

HandleService::HandleService() {
    _room_manager = new RoomManager();
    _user_manager = new UserManager();

    _msg_handle_map[CMsgType::LOGIN_MSG] = bind(&HandleService::Login, this, _1, _2, _3);

    _msg_handle_map[CMsgType::LOGINOUT_MSG] = bind(&HandleService::LoginOut, this, _1, _2, _3);

    _msg_handle_map[CMsgType::REG_MSG] = bind(&HandleService::Regist, this, _1, _2, _3);

    _msg_handle_map[CMsgType::CREATE_ROOM_MSG] = bind(&HandleService::CreateRoom, this, _1, _2, _3);

    _msg_handle_map[CMsgType::ADD_ROOM_MSG] = bind(&HandleService::AddRoom, this, _1, _2, _3);

    _msg_handle_map[CMsgType::LEAVE_ROOM_MSG] = bind(&HandleService::LeaveRoom, this, _1, _2, _3);

    _msg_handle_map[CMsgType::QUERY_ROOM_LIST_MSG] = bind(&HandleService::QueryRoomList, this, _1, _2, _3);

    _msg_handle_map[CMsgType::START_GAME] = bind(&HandleService::Login, this, _1, _2, _3);

    _msg_handle_map[CMsgType::QUERY_ROOM_USER_LIST_MSG] = bind(&HandleService::QueryPlayerInRoomList, this, _1, _2, _3);

    _msg_handle_map[CMsgType::START_GAME] = bind(&HandleService::StartGame, this, _1, _2, _3);

    _msg_handle_map[CMsgType::KeepLive] = bind(&HandleService::KeepLive, this);
}

HandleService::~HandleService() {
    delete _room_manager;
    delete _user_manager;
}

MsgHandler HandleService::GetHandler(int msgType) {
    auto it = _msg_handle_map.find(msgType);

    if (it == _msg_handle_map.end()) {
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp time) {
            LOG_ERROR << "msgid: " << msgType << "can not find handler";
        };
    } else {
        return _msg_handle_map[msgType];
    }
}

void HandleService::Regist(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    string name = js["name"];
    string password = js["password"];

    User user;
    user.set_name(name);
    user.set_password(password);

    bool state = _user_model.insert(user);
    if (state) {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.get_id();

        conn->send(response.dump());
    } else {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;

        conn->send(response.dump());
    }
}

void HandleService::Login(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    string name = js["name"];
    string passwd = js["password"];

    User user = _user_model.query(name);

    int id = user.get_id();

    if (user.get_name() == name && user.get_password() == passwd) {
        {
            // 登陆成功
            {
                lock_guard<mutex> lock(_conn_mutex);
                _user_connection_map.insert({id, conn});

                Player* p = new Player();
                InetAddress addr = conn.get()->localAddress();
                p->_userID = id;
                p->setIP(addr.toIp());
                p->setPasswd(passwd);
                p->setPort(addr.toPort());
                p->setUserName(user.get_name());
                // 用户管理，执行登录
                _user_manager->UserLogin(p);
            }

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.get_id();

            response["name"] = user.get_name();
            std::cout << user.get_name() << std::endl;

            std::string res = response.dump();
            conn->send(res);
        }
    } else {
        // 用户不存在或密码错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "id or password error";

        conn->send(response.dump());
    }
}

void HandleService::LoginOut(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int user_id = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_conn_mutex);
        auto it = _user_connection_map.find(user_id);
        if (it != _user_connection_map.end()) {
            _user_connection_map.erase(it);
        }

        Player* p = _user_manager->getPlayer(user_id);
        if (p) {
            uint64_t roomID = p->getRoomID();
            if (roomID != -1) {
                GameRoom* g = _room_manager->GetRoom(roomID);
                if (g->UserIsExistRoomID(user_id)) {
                    g->LeaveRoom(user_id);
                    _room_manager->update();
                }
            }
        }
        // 用户管理，执行离线
        _user_manager->UserLogout(user_id);
    }
}

void HandleService::CreateRoom(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int user_id = js["id"].get<int>();
    string name = js["room_name"];

    GameRoom* room = _room_manager->CreateRoom(name);
    Player* u = _user_manager->getPlayer(user_id);

    if (u) {
        uint64_t rid = u->getRoomID();
        if (rid != -1) {                     // 在房间里
            if (rid == room->getRoomID()) {  // 就在本房间里
                return;
            } else {  // 在其他房间里

                if (_room_manager->ExistRoom(rid)) {
                    _room_manager->GetRoom(rid)->LeaveRoom(user_id);
                    _room_manager->update();
                }
            }
        }
        u->CreateRoom(room->getRoomID());
        room->AddRoom(u);
    } else {
        LOG_ERROR << " user offline createroom  ";
    }
}

void HandleService::AddRoom(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int user_id = js["id"].get<int>();
    uint64_t room_id = js["room_id"].get<int>();
    string user_name = js["user_name"];

    // 先离开原房间
    uint64_t proom_id = _user_manager->getPlayer(user_id)->getRoomID();
    if (-1 != proom_id) {
        GameRoom* r = _room_manager->GetRoom(proom_id);
        r->LeaveRoom(user_id);
        _room_manager->update();
    }

    GameRoom* room = _room_manager->GetRoom(room_id);
    if (room) {
        room->getRoomStatus();
        if (!room->UserIsExistRoomID(user_id)) {
            room->AddRoom(_user_manager->getPlayer(user_id));
        }
    }

    json response;
    response["msgid"] = ADD_ROOM_MSG_ACK;
    response["add_user_id"] = user_id;
    response["add_user_name"] = user_name;

    string res = response.dump();

    auto players = room->getAllPlayer();
    for (int i = 0; i < players.size(); i++) {
        _user_connection_map[players[i]->_userID]->send(res);
    }
}

void HandleService::LeaveRoom(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int user_id = js["id"].get<int>();
    uint64_t room_id = js["roomid"].get<int>();

    GameRoom* room = _room_manager->GetRoom(room_id);
    if (room->UserIsExistRoomID(user_id)) {
        room->LeaveRoom(user_id);
        _room_manager->update();
        if (room->getPlayerNum() == 0) {
            _room_manager->DeleteRoom(room_id);
        }
    }
}

void HandleService::QueryPlayerInRoomList(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["user_id"].get<int>();
    uint64_t roomID = js["room_id"].get<uint64_t>();

    GameRoom* room = _room_manager->GetRoom(roomID);
    if (room) {
        json response;
        response["msgid"] = QUERY_ROOM_USER_LIST_MSG_ACK;
        response["room_id"] = roomID;
        std::unordered_map<uint64_t, std::string> roomlist;
        auto players = room->getAllPlayer();
        for (auto& item : players) {
            roomlist[item->_userID] = item->getUserName();
        }
        response["player_list"] = roomlist;
        std::string res = response.dump();
        conn->send(res);
    }
}

void HandleService::StartGame(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    uint64_t userid = js["user_id"].get<uint64_t>();

    GameRoom* room = _room_manager->GetRoom(_user_manager->getPlayer(userid)->getRoomID());

    auto players = room->getAllPlayer();
    json response;
    response["msgid"] = START_GAME_ACK;
    response["room_id"] = room->getRoomID();
    response["start"] = true;
    string res = response.dump();
    for (auto& item : players) {
        _user_connection_map[item->_userID]->send(res);
    }
}

void HandleService::QueryRoomList(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    uint64_t user_id = js["id"].get<int>();

    json response;
    response["msgid"] = QUERY_ROOM_LIST_MSG_ACK;
    response["id"] = user_id;

    std::unordered_map<uint64_t, std::string> rm = _room_manager->GetRoomList();

    response["room_list"] = rm;

    conn->send(response.dump());
}

void HandleService::ClientCLoseException(const TcpConnectionPtr& conn) {
    // 1.删除user_map    2.用户改为offline   3.线程安全
    User user;
    {
        lock_guard<mutex> lock(_conn_mutex);
        for (auto it = _user_connection_map.begin(); it != _user_connection_map.end(); it++) {
            if (it->second == conn) {
                user.set_id(it->first);
                _user_connection_map.erase(it);
                break;
            }
        }
    }
}

void HandleService::KeepLive() {
    cout << "============== ♥♥♥ ==============" << endl;
}

void HandleService::Reset() {
}
