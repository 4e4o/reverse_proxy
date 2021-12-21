#include "ClientProxySession.hpp"
#include "Application.hpp"

ClientProxySession::~ClientProxySession() {
}

void ClientProxySession::setSessionType(const uint8_t &sessionType) {
    m_sessionType = sessionType;
}

void ClientProxySession::startProxying(std::shared_ptr<ProxyDataSession> clientSession) {
    auto self = std::dynamic_pointer_cast<ClientProxySession>(shared_from_this());
    clientSession->startSSL(true, "server1", [self, clientSession]() {
        self->startHandshake(clientSession, self->m_sessionType, APP->serverId());
    });
}

void ClientProxySession::onHandshakeDone(TSession client) {
    // strip ssl layer
    client->socket().setSSL(false);
    auto clientSession = std::dynamic_pointer_cast<ProxyDataSession>(client);
    ProxySession::startProxying(clientSession);
}
