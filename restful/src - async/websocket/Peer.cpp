#include "Peer.hpp"

#include <QString>
#include <QTextCodec>
#include <iostream>

#include "Room.hpp"
#include "dflogger.h"
#include "eprotmmictrl.h"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

void Peer::sendMessage(const oatpp::String& message) {
    class SendMessageCoroutine : public oatpp::async::Coroutine<SendMessageCoroutine> {
       private:
        Peer* m_peer;

       public:
        SendMessageCoroutine(Peer* peer) : m_peer(peer){};
        Action act() override {
            while (true) {
                oatpp::async::LockGuard lock(&m_peer->m_messageQueueMutex);
                if (m_peer->m_messageQueue.empty()) {
                    break;
                }
                oatpp::String message = m_peer->m_messageQueue.front();
                m_peer->m_messageQueue.pop_front();

                // 等待发送完成后再发送下一条消息
                return oatpp::async::synchronize(&m_peer->m_writeLock, m_peer->m_socket->sendOneFrameTextAsync(message))
                    .next(yieldTo(&SendMessageCoroutine::act));
            }

            return finish();
        };
    };

    oatpp::async::LockGuard lock(&m_messageQueueMutex);
    m_messageQueue.push_back(message);
    m_asyncExecutor->execute<SendMessageCoroutine>(this);
}

/**
 * @brief 召唤定值
 *
 * @param para
 */
void Peer::executeCallDZ(const oatpp::Object<FixValueRequestDto>& para) {
    class CallDZCoroutine : public oatpp::async::Coroutine<CallDZCoroutine> {
       private:
        Peer* m_peer;

       public:
        CallDZCoroutine(Peer* peer) : m_peer(peer) {}

        Action act() override {
            ProgressCallback progressCallback = [&](const QString& progress) {
                if (m_peer != nullptr) {
                    DFLOG_DEBUG("%s recv: %s", m_peer->m_id->c_str(), progress.toAscii().data());
                    m_peer->sendMessage(progress.toStdString());
                }
            };
            executeBatch(progressCallback);
            return finish();
        }
    };

    m_asyncExecutor->execute<CallDZCoroutine>(this);
}

std::shared_ptr<Room> Peer::getRoom() { return m_room; }

oatpp::String Peer::getUserId() { return m_id; }

oatpp::async::CoroutineStarter Peer::onPing(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) {
    return oatpp::async::synchronize(&m_writeLock, socket->sendPongAsync(message));
}

oatpp::async::CoroutineStarter Peer::onPong(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) {
    return nullptr; // do nothing
}

oatpp::async::CoroutineStarter Peer::onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) {
    // DFLOG_DEBUG("onClose");
    return nullptr; // do nothing
}

oatpp::async::CoroutineStarter Peer::readMessage(const std::shared_ptr<AsyncWebSocket>& socket,
                                                 v_uint8 opcode, p_char8 data,
                                                 oatpp::v_io_size size) {
    if (size == 0) { // message transfer finished

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.setCurrentPosition(0);
        DFLOG_DEBUG("%s recv: message=\n%s", m_id->c_str(), wholeMessage->c_str());

        auto jsonObjectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        if (m_room->getName() == "fixvalue") {
            auto fixvalueReqDto = jsonObjectMapper->readFromString<oatpp::Object<FixValueRequestDto>>(wholeMessage);
            DFLOG_DEBUG("param:{optype=%s, substation=%s, round=%d, area=%d}",
                        fixvalueReqDto->optype->c_str(),
                        fixvalueReqDto->para->substation->c_str(),
                        *(fixvalueReqDto->para->area),
                        *(fixvalueReqDto->para->round));

            // 执行召唤定值
            executeCallDZ(fixvalueReqDto);
        }

    } else if (size > 0) { // message frame received
        m_messageBuffer.writeSimple(data, size);
    }

    return nullptr; // do nothing
}