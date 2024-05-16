#include "Player.h"

Player::Player() {
    _roomID = -1;
    _owner = false;
}

Player::~Player() {
}

void Player::CreateRoom(uint64_t rID) {
    _roomID = rID;
    _owner = true;
}

void Player::EntryRoom(uint64_t rID) {
    _roomID = rID;
    _owner = false;
}

void Player::LeaveRoom(uint64_t rID) {
    _roomID = -1;
    _owner = false;
}

const uint64_t Player::getRoomID() {
    return _roomID;
}

void Player::setRoomID(uint64_t rID) {
    _roomID = rID;
}

bool Player::IsOwner() {
    return _owner;
}
