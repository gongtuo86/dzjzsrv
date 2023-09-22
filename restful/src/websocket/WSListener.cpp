
#include "WSListener.hpp"

#include <future>
#include <thread>
#include <QDateTime>

#include "ThreadPool.h"
#include "commonutils.h"
#include "dflogger.h"
#include "dzjzprot.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSListener

void WSListener::onPing(const WebSocket& socket, const oatpp::String& message)
{
    // DFLOG_DEBUG("onPing");
    socket.sendPong(message);
}

void WSListener::onPong(const WebSocket& socket, const oatpp::String& message)
{
    // DFLOG_DEBUG("onPong");
}

void WSListener::onClose(const WebSocket& socket, v_uint16 code,
                         const oatpp::String& message)
{
    m_stopEventThread = true; // 设置线程停止标志
    if (m_eventThread.joinable())
    {
        m_eventThread.join(); // 等待线程结束
        DFLOG_DEBUG("[%s] 实时事件订阅结束", m_id->c_str());
    }
    DFLOG_DEBUG("onClose code=%d", code);
}

void WSListener::handleRealEvent()
{
    int count = 150;
    while (!m_stopEventThread)
    {
        dfJson::Value jMessage = getRealEvent();
        if (!jMessage.empty())
        {
            std::string strMessage = jsonToString(jMessage);
            DFLOG_DEBUG("[%s] [告警事项] %s", m_id->c_str(), strMessage.c_str());
            sendMessage(gb2312ToUtf8(strMessage));
            count = 0; // 有实际事件，重置计数器
        }
        else
        {
            if (++count >= 150) // 无实际事件，且已经等待了30秒
            {
                dfJson::Value jResponse;
                jResponse["optype"] = "getRealEvent";
                jResponse["info"] = "低周减载事项测试";
                QDateTime dateTime = QDateTime::currentDateTime();
                jResponse["time"] = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz").toAscii().data();
                jResponse["substation"] = "测试站"; // 厂站
                jResponse["state"] = "动作";

                // 发送测试事件
                std::string strMessage = jsonToString(jResponse);
                sendMessage(gb2312ToUtf8(strMessage));
                DFLOG_DEBUG("[%s] [告警事项] %s", m_id->c_str(), strMessage.c_str());
                count = 0; // 发送测试事件后，重置计数器
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

void WSListener::readMessage(const WebSocket& socket, v_uint8 opcode,
                             p_char8 data, oatpp::v_io_size size)
{
    if (size == 0)
    { // message transfer finished

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.setCurrentPosition(0);

        if (wholeMessage->empty())
            return;

        DFLOG_DEBUG("[%s] onMessage message='%s'", m_id->c_str(), wholeMessage->c_str());

        std::string gb2312Parameter = utf8ToGb2312(wholeMessage->c_str());

        dfJson::Value jPara;
        dfJson::CharReaderBuilder readerBuilder;
        std::istringstream iss(gb2312Parameter);
        dfJson::parseFromStream(readerBuilder, iss, &jPara, nullptr);

        // 事项订阅
        if (jPara["optype"] == "getRealEvent")
        {
            if (m_eventThread.joinable())
            {
                m_stopEventThread = true;
                m_eventThread.join();
                DFLOG_DEBUG("[%s] 已有事件订阅, 重新订阅", m_id->c_str());
            }
            m_stopEventThread = false; // 确保线程停止标志为 false
            m_eventThread = std::thread(&WSListener::handleRealEvent, this);
            DFLOG_DEBUG("[%s] 实时事件订阅成功", m_id->c_str());
        }
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
    // DFLOG_DEBUG("threadid=%lu", std::this_thread::get_id());
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

        // 停止事件处理线程
        peer->m_stopEventThread = true; // 设置线程停止标志
        if (peer->m_eventThread.joinable())
        {
            peer->m_eventThread.join(); // 等待线程结束
            DFLOG_DEBUG("[%s] 实时事件订阅结束", id->c_str());
        }

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