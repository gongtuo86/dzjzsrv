#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "websocket/Lobby.hpp"

// �����ʼ����
class AppComponent {
   public:
    /**
     * Create Async Executor
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)
    ([] {
        return std::make_shared<oatpp::async::Executor>(
            4 /* Data-Processing threads */, 1 /* I/O threads */, 1 /* Timer threads */
        );
    }());

    // �����ṩTCP���ӡ���������IP��ַ��8000�˿ڵ�TCP�����ṩ����
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
    ([] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared(
            {"0.0.0.0", 8000, oatpp::network::Address::IP_4});
    }());

    // ����·�����
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
    ([] { return oatpp::web::server::HttpRouter::createShared(); }());

    // �������Ӵ����������������ʹ��·���������·������
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
    ("http", [] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                        router);  // get Router component
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>,
                        executor);  // get Async executor component
        return oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
    }());

    // ����ObjectMapper���������Contoller��API�����л�/�����л�DTO
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)
    ([] { return oatpp::parser::json::mapping::ObjectMapper::createShared(); }());

    // ����WebSocket���Ӵ����������������ʹ��·���������·������
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)
    ("websocket", [] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
        auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
        connectionHandler->setSocketInstanceListener(std::make_shared<Lobby>());
        return connectionHandler;
    }());
};

#endif /* AppComponent_hpp */