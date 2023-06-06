#ifndef WSListener_hpp
#define WSListener_hpp

#include "ThreadPool.h"
#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp-websocket/WebSocket.hpp"

/**
 * WebSocket listener listens on incoming WebSocket events.
 */
class WSListener : public oatpp::websocket::WebSocket::Listener
{
private:
    static constexpr const char* TAG = "Server_WSListener";

private:
    oatpp::data::stream::BufferOutputStream m_messageBuffer;
    oatpp::String m_id;
    const WebSocket* m_socket;

public:
    WSListener(const WebSocket* socket, const oatpp::String& id) : m_socket(socket), m_id(id){};
    void onPing(const WebSocket& socket, const oatpp::String& message) override;
    void onPong(const WebSocket& socket, const oatpp::String& message) override;
    void onClose(const WebSocket& socket, v_uint16 code, const oatpp::String& message) override;
    void readMessage(const WebSocket& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
    oatpp::String getId();
    void sendMessage(const oatpp::String& message);
};

/**
 * Listener on new WebSocket connections.
 */
class WSInstanceListener : public oatpp::websocket::ConnectionHandler::SocketInstanceListener
{
private:
    static constexpr const char* TAG = "Server_WSInstanceListener";

private:
public:
    void onAfterCreate(const oatpp::websocket::WebSocket& socket, const std::shared_ptr<const ParameterMap>& params) override;
    void onBeforeDestroy(const oatpp::websocket::WebSocket& socket) override;
};

extern std::unordered_map<std::string, std::shared_ptr<WSListener>> g_peerById;
extern std::mutex g_peerByIdLock;

extern std::unordered_map<std::string, std::shared_ptr<TaskState>> g_tasks;
extern std::mutex g_tasksLock;

#endif // WSListener_hpp
