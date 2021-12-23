#include "ClientProxySession.hpp"
#include "Application.hpp"

ClientProxySession::~ClientProxySession() {
//    AAP->log("ClientProxySession::~ClientProxySession %p", this);
}

void ClientProxySession::setSessionType(const uint8_t &sessionType) {
    m_sessionType = sessionType;
}

void ClientProxySession::startProxying(std::shared_ptr<ProxyDataSession> clientSession, bool) {
    auto self = Session::shared_from_this<ClientProxySession>();
    clientSession->startSSL(true, "server1", [self, clientSession]() {
        self->startHandshake(clientSession, self->m_sessionType, APP->serverId());
    });
}

void ClientProxySession::onHandshakeDone(TSession client) {
    // strip ssl layer
    client->socket().setSSL(false);
    auto clientSession = std::dynamic_pointer_cast<ProxyDataSession>(client);
    ProxyDataSession::startProxying(clientSession);
}
