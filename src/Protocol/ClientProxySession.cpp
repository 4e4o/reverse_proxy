#include "ClientProxySession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Protocol/ClientHandshake.hpp"
#include "Application.hpp"

ClientProxySession::ClientProxySession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock)
    : ProxySession(io_service, std::move(sock)),
      m_sessionType(0) {
}

ClientProxySession::~ClientProxySession() {
}

void ClientProxySession::setSessionType(const uint8_t &sessionType) {
    m_sessionType = sessionType;
}

void ClientProxySession::startProxying(std::shared_ptr<ProxyDataSession>) {
    auto self = std::dynamic_pointer_cast<ClientProxySession>(shared_from_this());
    auto client = std::dynamic_pointer_cast<ProxyDataSession>(this->client()->session());

    // не запускаем проксирование сразу
    // сначала надо наш протокол запустить с прокси сервером
    std::shared_ptr<ClientHandshake> h(new ClientHandshake(m_sessionType, APP->serverId()));

    h->onSuccessResponse.connect_extended([self, client](const boost::signals2::connection &c) {
        c.disconnect();
        self->ProxySession::startProxying(client);
    });

    h->start(client);
}
