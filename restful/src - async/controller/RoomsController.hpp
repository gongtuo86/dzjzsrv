
#ifndef RoomsController_hpp
#define RoomsController_hpp

#include "oatpp-websocket/Handshaker.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/ConnectionHandler.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "dto/dtos.hpp"

#include <sstream>

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- codegen begin

/**
 * @brief Éú³Ésocketid
 * 
 * @param length 
 * @return std::string 
 */
std::string generateSocketID() {
    size_t length = 64;
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::stringstream socketid;
    for (size_t i = 0; i < length; ++i) {
        socketid << characters[distribution(generator)];
    }

    return socketid.str();
}

/**
 * Controller with WebSocket-connect endpoint.
 */
class RoomsController : public oatpp::web::server::api::ApiController {
   private:
    typedef RoomsController __ControllerType;

   private:
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler, "websocket");

   public:
    RoomsController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper) {}

   public:
    ENDPOINT_ASYNC("GET", "/", Root) {
        ENDPOINT_ASYNC_INIT(Root)

        const char* pageTemplate =
            "<html lang='en'>"
            "<head>"
            "<meta charset=utf-8/>"
            "</head>"
            "<body>"
            "<p>Hello Async WebSocket Rooms Server!</p>"
            "<p>Connect to chat room:</p>"
            "<code>localhost:8000/ws/chat/{room_name}/?nickname={nickname}</"
            "code>"
            "</body>"
            "</html>";

        Action act() override { return _return(controller->createResponse(Status::CODE_200, pageTemplate)); }
    };

    ENDPOINT_ASYNC("GET", "ws/{room}/*", WS){
        ENDPOINT_ASYNC_INIT(WS)

        Action act() override{

            auto roomName = request->getPathVariable("room");
            auto id = request->getHeader("Sec-WebSocket-Key");

            /* Websocket handshake */
            auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(),
                                                                                controller->websocketConnectionHandler);
          
            auto parameters = std::make_shared<oatpp::network::ConnectionHandler::ParameterMap>();

            (*parameters)["room"] = roomName;
            (*parameters)["id"] = id;

            /* Set connection upgrade params */
            response->setConnectionUpgradeParameters(parameters);

            return _return(response);
        }
    };


// TODO Insert Your endpoints here !!!
};

#include OATPP_CODEGEN_END(ApiController) //<-- codegen end

#endif /* RoomsController_hpp */
