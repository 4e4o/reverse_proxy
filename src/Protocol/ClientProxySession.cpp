#include "ClientProxySession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Protocol/ClientHandshake.hpp"
#include "Application.hpp"

ClientProxySession::~ClientProxySession() {
}

void ClientProxySession::setSessionType(const uint8_t &sessionType) {
    m_sessionType = sessionType;
}

void ClientProxySession::startProxying(std::shared_ptr<ProxyDataSession> client) {
    auto self = std::dynamic_pointer_cast<ClientProxySession>(shared_from_this());

    // не запускаем проксирование сразу
    // сначала надо наш протокол запустить с прокси сервером
    std::shared_ptr<ClientHandshake> h(new ClientHandshake(m_sessionType, APP->serverId()));

    h->onSuccessResponse.connect_extended([self, client](const boost::signals2::connection &c) {
        c.disconnect();
        self->ProxySession::startProxying(client);
    });

    h->start(client);
}
