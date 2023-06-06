
#include "Room.hpp"

void Room::addPeer(const std::shared_ptr<Peer>& peer) {
    std::lock_guard<std::mutex> guard(m_peerByIdLock);
    m_peerById[peer->getUserId()] = peer;
}

void Room::removePeerById(const oatpp::String& Id) {
    std::lock_guard<std::mutex> guard(m_peerByIdLock);
    m_peerById.erase(Id);
}

oatpp::String Room::getName() {
    return m_name;
}

void Room::sendMessage(const oatpp::String& message) {
    std::lock_guard<std::mutex> guard(m_peerByIdLock);
    for (auto& pair : m_peerById) {
        pair.second->sendMessage(message);
    }
}