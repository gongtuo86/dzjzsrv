
#ifndef ASYNC_SERVER_ROOMS_ROOM_HPP
#define ASYNC_SERVER_ROOMS_ROOM_HPP

#include <unordered_map>

#include "Peer.hpp"

class Room {
   private:
    oatpp::String m_name;
    std::unordered_map<oatpp::String, std::shared_ptr<Peer>> m_peerById;
    std::mutex m_peerByIdLock;

   public:
    Room(const oatpp::String& name) : m_name(name) {}

    /**
     * Add peer to the room.
     * @param peer
     */
    void addPeer(const std::shared_ptr<Peer>& peer);

    /**
     * Remove peer from the room.
     * @param userId
     */
    void removePeerById(const oatpp::String& userId);

    /**
     * Send message to all peers in the room.
     * @param message
     */
    void sendMessage(const oatpp::String& message);

    oatpp::String getName();
};

#endif // ASYNC_SERVER_ROOMS_ROOM_HPP
