#include "Lobby.hpp"
#include "dflogger.h"

v_int32 Lobby::obtainNewUserId() { return m_userIdCounter++; }

std::shared_ptr<Room> Lobby::getOrCreateRoom(const oatpp::String& roomName) {
    std::lock_guard<std::mutex> lock(m_roomsMutex);
    std::shared_ptr<Room>& room = m_rooms[roomName];
    if (!room) {
        room = std::make_shared<Room>(roomName);
    }
    return room;
}

// websocket连接处理
void Lobby::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket,
                                      const std::shared_ptr<const ParameterMap>& params) {
    auto roomName = params->find("room")->second;
    auto id = params->find("id")->second;
    auto room = getOrCreateRoom(roomName);

    auto peer = std::make_shared<Peer>(socket, room, id);
    socket->setListener(peer);

    room->addPeer(peer);

    DFLOG_INFO("%s connect %s", id->c_str(), roomName->c_str());
}

// websocket连接断开处理
void Lobby::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) {
    auto peer = std::static_pointer_cast<Peer>(socket->getListener());
    auto id = peer->getUserId();
    auto room = peer->getRoom();

    room->removePeerById(id);

    DFLOG_INFO("%s disconnect %s", id->c_str(), room->getName()->c_str());

    /* Remove circle `std::shared_ptr` dependencies */
    socket->setListener(nullptr);
}