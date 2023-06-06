
#ifndef MyController_hpp
#define MyController_hpp

#include <QDateTime>

#include "commonutils.h"
#include "dflogger.h"
#include "dzjzprot.h"
#include "oatpp-websocket/Handshaker.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "websocket/WSListener.hpp"

extern ThreadPool g_threadPool;
extern std::unordered_map<std::string, std::shared_ptr<TaskState>> g_tasks; // ��������

#include OATPP_CODEGEN_BEGIN(ApiController)                                 //<-- codegen begin

// ����һ����������������� g_peerById ִ�иò���
template <typename Function>
bool performWithPeer(const std::string &id, Function function)
{
    std::unique_lock<std::mutex> lock(g_peerByIdLock);
    auto it = g_peerById.find(id);
    if (it != g_peerById.end())
    {
        function(it->second);
        return true;
    }
    return false;
}

bool isPeerConnected(const std::string &id)
{
    std::lock_guard<std::mutex> lock(g_peerByIdLock);
    return g_peerById.find(id) != g_peerById.end();
}

// ������Ӧ
dfJson::Value constructJsonResponse(int status, const std::string &message)
{
    dfJson::Value root;
    root["status"] = status;
    root["message"] = message;
    return root;
}

std::shared_ptr<TaskState> createNewTaskState(const std::string &id)
{
    std::lock_guard<std::mutex> lock(g_tasksLock);
    auto it = g_tasks.find(id);
    if (it != g_tasks.end())
    {
        dfJson::Value root = constructJsonResponse(409, "����������ִ��");
        std::string rstr = jsonToString(root);
        DFLOG_DEBUG("id=%s %s", id.c_str(), rstr.c_str());
        throw std::runtime_error(gb2312ToUtf8(rstr));
    }
    auto state = std::make_shared<TaskState>();
    g_tasks[id] = state;
    DFLOG_DEBUG("[%s] add task count=%d", id.c_str(), g_tasks.size());
    return state;
}

/**
 * Controller with WebSocket-connect endpoint.
 */
class MyController : public oatpp::web::server::api::ApiController
{
private:
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                    websocketConnectionHandler, "websocket");

public:
    MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {
    }

public:
    ENDPOINT("GET", "ws", ws,
             REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto id = generateSocketID();
        auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(), websocketConnectionHandler);
        auto parameters = std::make_shared<oatpp::network::ConnectionHandler::ParameterMap>();

        // ����socketid
        (*parameters)["socketid"] = id;
        response->setConnectionUpgradeParameters(parameters);
        response->putHeader("socketid", id);

        return response;
    }

    ADD_CORS(inspectDevice, "*", "*", "*", "1728000")
    ENDPOINT("POST", "/api/inspectDevice", inspectDevice,
             HEADER(String, id, "socketid"), BODY_STRING(String, inputParameter))
    {
        std::string gb2312Parameter = utf8ToGb2312(inputParameter);
        DFLOG_DEBUG("id=%s para=%s", id->c_str(), inputParameter->c_str());

        if (!isPeerConnected(id))
        {
            dfJson::Value root = constructJsonResponse(400, "websocket �ѶϿ�");
            std::string rstr = jsonToString(root);
            DFLOG_DEBUG("[%s]%s", id->c_str(), rstr.c_str());
            return createResponse(Status::CODE_400, gb2312ToUtf8(rstr));
        }

        g_threadPool.enqueue(
            [id, gb2312Parameter]()
            {
                DFLOG_DEBUG("[%s]װ��Ѳ����������̳߳�", id->c_str());
                executeInspectionByCond(gb2312Parameter, [&](const std::string &progress)
                                        {
                                            performWithPeer(id, [&](std::shared_ptr<WSListener> &peer)
                                                            {
                                                                DFLOG_DEBUG("[%s]%s", id->c_str(), progress.c_str());
                                                                peer->sendMessage(gb2312ToUtf8(progress));
                                                            });
                                        });
                DFLOG_DEBUG("[%s]װ��Ѳ��������̳߳�", id->c_str());
            });

        // �������������
        dfJson::Value jResponse = constructJsonResponse(200, "OK");
        DFLOG_DEBUG("[%s]%s", id->c_str(), jsonToString(jResponse).c_str());
        return createResponse(Status::CODE_200, gb2312ToUtf8(jsonToString(jResponse)));
    }

    ADD_CORS(CentrialDevice, "*", "*", "*", "1728000")
    ENDPOINT("POST", "/api/CentrialDevice", CentrialDevice,
             HEADER(String, id, "socketid"), BODY_STRING(String, inputParameter))
    {
        std::string gb2312Parameter = utf8ToGb2312(inputParameter);
        DFLOG_DEBUG("id=%s para=%s", id->c_str(), inputParameter->c_str());

        if (!isPeerConnected(id))
        {
            dfJson::Value root = constructJsonResponse(400, "websocket �ѶϿ�");
            std::string rstr = jsonToString(root);
            DFLOG_DEBUG("[%s]%s", id->c_str(), rstr.c_str());
            return createResponse(Status::CODE_400, gb2312ToUtf8(rstr));
        }

        try
        {
            std::shared_ptr<TaskState> state = createNewTaskState(id);
            state->future = g_threadPool.enqueue(
                [id, gb2312Parameter, state]()
                {
                    // ��ÿ��ѭ���Ŀ�ʼ������Ƿ�Ӧ��ֹͣ
                    while (!*state->shouldStop)
                    {
                        DFLOG_DEBUG("[%s] ����ʽװ��ˢ��", id->c_str());
                        dfJson::Value jMessage = getCentrialDevice(gb2312Parameter);

                        std::string strMessage = jsonToString(jMessage);
                        performWithPeer(id, [&](std::shared_ptr<WSListener> &peer)
                                        {
                                            peer->sendMessage(gb2312ToUtf8(strMessage));
                                        });
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                    }
                    DFLOG_DEBUG("[%s] ����ʽװ��ˢ��ֹͣ", id->c_str());
                });

            // �������������
            dfJson::Value jResponse = constructJsonResponse(200, "OK");
            return createResponse(Status::CODE_200, gb2312ToUtf8(jsonToString(jResponse)));
        }
        catch (const std::runtime_error &e)
        {
            return createResponse(Status::CODE_409, e.what());
        }
    }

    ADD_CORS(SubscribeRealEvent, "*", "*", "*", "1728000")
    ENDPOINT("POST", "/api/SubscribeRealEvent", SubscribeRealEvent,
             HEADER(String, id, "socketid"))
    {
        if (!isPeerConnected(id))
        {
            dfJson::Value root = constructJsonResponse(400, "websocket �ѶϿ�");
            std::string rstr = jsonToString(root);
            DFLOG_DEBUG("[%s]%s", id->c_str(), rstr.c_str());
            return createResponse(Status::CODE_400, gb2312ToUtf8(rstr));
        }

        try
        {
            std::shared_ptr<TaskState> state = createNewTaskState(id);
            state->future = g_threadPool.enqueue(
                [id, state]()
                {
                    // ��ÿ��ѭ���Ŀ�ʼ������Ƿ�Ӧ��ֹͣ
                    DFLOG_DEBUG("[%s] �澯����ˢ�¿�ʼ", id->c_str());
                    int count = 0;
                    while (!*state->shouldStop)
                    {
                        dfJson::Value jMessage = getRealEvent();
                        if (!jMessage.empty())
                        {
                            std::string strMessage = jsonToString(jMessage);
                            performWithPeer(id, [&](std::shared_ptr<WSListener> &peer)
                                            {
                                                DFLOG_DEBUG("[�澯����] %s", strMessage.c_str());
                                                peer->sendMessage(gb2312ToUtf8(strMessage));
                                            });
                            count = 0; // ��ʵ���¼������ü�����
                        }
                        else
                        {
                            if (++count >= 15) // ��ʵ���¼������Ѿ��ȴ���3��
                            {
                                dfJson::Value jResponse;
                                jResponse["optype"] = "getRealEvent";
                                jResponse["info"] = "���ܼ����������";
                                QDateTime dateTime = QDateTime::currentDateTime();
                                jResponse["time"] = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz").toAscii().data();
                                jResponse["substaion"] = "վ��ϵͳ����վ"; // ��վ
                                jResponse["state"] = "����";

                                // ���Ͳ����¼�
                                std::string strMessage = jsonToString(jResponse);
                                performWithPeer(id, [&](std::shared_ptr<WSListener> &peer)
                                                {
                                                    peer->sendMessage(gb2312ToUtf8(strMessage));
                                                    DFLOG_DEBUG("[�澯����] %s", strMessage.c_str());
                                                });
                                count = 0; // ���Ͳ����¼������ü�����
                            }
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    }
                    DFLOG_DEBUG("[%s] �澯����ˢ��ֹͣ", id->c_str());
                });

            // �������������
            dfJson::Value jResponse = constructJsonResponse(200, "OK");
            return createResponse(Status::CODE_200, gb2312ToUtf8(jsonToString(jResponse)));
        }
        catch (const std::runtime_error &e)
        {
            return createResponse(Status::CODE_409, e.what());
        }
    }
};

#include OATPP_CODEGEN_END(ApiController) //<-- codegen end

#endif
