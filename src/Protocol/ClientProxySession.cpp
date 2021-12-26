#include "ClientProxySession.hpp"
#include "Application.hpp"
#include "Config.hpp"

ClientProxySession::~ClientProxySession() {
//    AAP->log("ClientProxySession::~ClientProxySession %p", this);
}

void ClientProxySession::setSessionType(const uint8_t &sessionType) {
    m_sessionType = sessionType;
}

void ClientProxySession::startProxying(std::shared_ptr<ProxyDataSession> clientSession, bool) {
    auto self = Session::shared_from_this<ClientProxySession>();
    clientSession->socket().setSSLParameters(CONFIG.verifyHost, CONFIG.keysPath);
    clientSession->startSSL(true, [self, clientSession]() {
        self->startHandshake(clientSession, self->m_sessionType, CONFIG.serverId, true);
    });
}

void ClientProxySession::onHandshakeDone(TSession client) {
    auto clientSession = std::dynamic_pointer_cast<ProxyDataSession>(client);
    ProxyDataSession::startProxying(clientSession);
}
