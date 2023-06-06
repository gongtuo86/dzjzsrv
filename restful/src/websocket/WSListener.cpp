
#include "WSListener.hpp"

#include <future>
#include <thread>

#include "ThreadPool.h"
#include "commonutils.h"
#include "dflogger.h"
#include "dzjzprot.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSListener

void WSListener::onPing(const WebSocket& socket, const oatpp::String& message)
{
    DFLOG_DEBUG("onPing");
    socket.sendPong(message);
}

void WSListener::onPong(const WebSocket& socket, const oatpp::String& message)
{
    DFLOG_DEBUG("onPong");
}

void WSListener::onClose(const WebSocket& socket, v_uint16 code,
                         const oatpp::String& message)
{
    DFLOG_DEBUG("onClose code=%d", code);
    // auto peer = std::static_pointer_cast<WSListener>(socket.getListener());
    // if (peer)
    // {
    //     auto id = peer->getId();

    //     {
    //         std::lock_guard<std::mutex> lock(g_tasksLock);
    //         auto it = g_tasks.find(id);
    //         if (it != g_tasks.end())
    //         {
    //             it->second->stop();
    //             g_tasks.erase(it);
    //             DFLOG_DEBUG("Õ∆ÀÕ»ŒŒÒ count=%d", g_tasks.size());
    //         }
    //     }

    //     {
    //         std::lock_guard<std::mutex> lock(g_peerByIdLock);
    //         g_peerById.erase(id);
    //     }
    //     DFLOG_DEBUG("Connection closed. Connection count=%d id=%s", g_peerById.size(), id->c_str());
    // }
}

void WSListener::readMessage(const WebSocket& socket, v_uint8 opcode,
                             p_char8 data, oatpp::v_io_size size)
{
    if (size == 0)
    { // message transfer finished

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.setCurrentPosition(0);

        // OATPP_LOGD(TAG, "onMessage message='%s'", wholeMessage->c_str());

        /* Send message in reply */
        // socket.sendOneFrameText("Hello from oatpp!: " + wholeMessage);
    }
    else if (size > 0)
    { // message frame received
        m_messageBuffer.writeSimple(data, size);
    }
}

void WSListener::sendMessage(const oatpp::String& message)
{
    if (m_socket)
    {
        m_socket->sendOneFrameText(message);
        // DFLOG_DEBUG("threadid=%lu", std::this_thread::get_id());
    }
}

oatpp::String WSListener::getId() { return m_id; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSInstanceListener

void WSInstanceListener::onAfterCreate(
    const oatpp::websocket::WebSocket& socket,
    const std::shared_ptr<const ParameterMap>& params)
{
    auto id = params->find("socketid")->second;
    auto peer = std::make_shared<WSListener>(&socket, id);
    {
        std::lock_guard<std::mutex> lock(g_peerByIdLock);
        g_peerById.emplace(id, peer);
        socket.setListener(peer);
    }

    DFLOG_DEBUG("New Incoming Connection. Connection count=%d id=%s", g_peerById.size(), id->c_str());
    dfJson::Value jMessage;
    jMessage["socketid"] = id->c_str();

    peer->sendMessage(jsonToString(jMessage));
}

void WSInstanceListener::onBeforeDestroy(
    const oatpp::websocket::WebSocket& socket)
{
    auto peer = std::static_pointer_cast<WSListener>(socket.getListener());
    if (peer)
    {
        auto id = peer->getId();

        {
            std::lock_guard<std::mutex> lock(g_tasksLock);
            auto it = g_tasks.find(id);
            if (it != g_tasks.end())
            {
                it->second->stop();
                g_tasks.erase(it);
                DFLOG_DEBUG("[%s] delete task count=%d", id->c_str(), g_tasks.size());
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_peerByIdLock);
            g_peerById.erase(id);
        }
        DFLOG_DEBUG("Connection closed. Connection count=%d id=%s", g_peerById.size(), id->c_str());
    }
}