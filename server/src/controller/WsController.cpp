#include <server/controller/WsController.h>
#include <server/chat/WsRequestProcessor.h>
#include <server/chat/MessageHandlerService.h>
#include <server/chat/MessageHandlers.h>
#include <server/chat/WsData.h>
#include <server/chat/ChatRoomManager.h>
#include <common/utils/utils.h>

WsController::WsController() {
    LOG_INFO << "Constructing WsController service chain...";

    auto dbClient = drogon::app().getDbClient();
    if (!dbClient) {
        LOG_FATAL << "Database client is not available! Aborting.";
        drogon::app().quit();
        return;
    }

    auto handlers = std::make_unique<MessageHandlers>(dbClient);
    auto dispatcher = std::make_unique<MessageHandlerService>(std::move(handlers));
    m_requestProcessor = std::make_unique<WsRequestProcessor>(std::move(dispatcher));

    LOG_INFO << "WsController service chain constructed.";
}

WsController::~WsController() = default;

void WsController::handleNewConnection(const drogon::HttpRequestPtr& req, const drogon::WebSocketConnectionPtr& conn) {
    LOG_TRACE << "WS connect: " << conn->peerAddr().toIpPort();
    conn->setContext(std::make_shared<WsData>());
    chat::Envelope helloEnv;
    helloEnv.mutable_server_hello()->set_type(chat::ServerType::TYPE_SERVER);
    sendEnvelope(conn, helloEnv);
}

void WsController::handleNewMessage(const drogon::WebSocketConnectionPtr& conn, std::string&& msg_str, const drogon::WebSocketMessageType& type) {
    if(type != drogon::WebSocketMessageType::Binary) {
        LOG_TRACE << "Non-binary WS message received from " << conn->peerAddr().toIpPort() << ". Ignoring.";
        return;
    }

    drogon::async_run(std::bind(&WsRequestProcessor::handleIncomingMessage, m_requestProcessor.get(), std::move(conn), std::move(msg_str)));
}

void WsController::handleConnectionClosed(const drogon::WebSocketConnectionPtr& conn) {
    LOG_TRACE << "WS closed: " << conn->peerAddr().toIpPort();
    ChatRoomManager::instance().unregisterConnection(conn);
}
