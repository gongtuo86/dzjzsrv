
#ifndef ASYNC_SERVER_ROOMS_PEER_HPP
#define ASYNC_SERVER_ROOMS_PEER_HPP

#include <deque>
#include <mutex>

#include "dto/dtos.hpp"
#include "oatpp-websocket/AsyncWebSocket.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/async/Lock.hpp"
#include "oatpp/core/macro/component.hpp"

class Room; // FWD

class Peer : public oatpp::websocket::AsyncWebSocket::Listener {
   private:
    oatpp::data::stream::BufferOutputStream m_messageBuffer;
    oatpp::async::Lock m_writeLock;
    std::shared_ptr<AsyncWebSocket> m_socket;
    std::shared_ptr<Room> m_room;
    oatpp::String m_id;
    std::deque<oatpp::String> m_messageQueue;
    oatpp::async::Lock m_messageQueueMutex;

   private:
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);

   public:
    Peer(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<Room>& room,
         const oatpp::String& id)
        : m_socket(socket), m_room(room), m_id(id) {}
    void sendMessage(const oatpp::String& message);
    void executeCallDZ(const oatpp::Object<FixValueRequestDto>& para);
    std::shared_ptr<Room> getRoom();
    oatpp::String getUserId();

   public:
    CoroutineStarter onPing(const std::shared_ptr<AsyncWebSocket>& socket,
                            const oatpp::String& message) override;
    CoroutineStarter onPong(const std::shared_ptr<AsyncWebSocket>& socket,
                            const oatpp::String& message) override;
    CoroutineStarter onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code,
                             const oatpp::String& message) override;
    CoroutineStarter readMessage(const std::shared_ptr<AsyncWebSocket>& socket, v_uint8 opcode,
                                 p_char8 data, oatpp::v_io_size size) override;
};

#endif // ASYNC_SERVER_ROOMS_PEER_HPP
