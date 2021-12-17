#include "ProxySession.hpp"
#include "Base/Network/Client.hpp"

ProxySession::ProxySession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock)
    : ProxyDataSession(io_service, std::move(sock)),
      m_outgoing(new Client(io_service)) {
}

ProxyDataSession* ProxySession::createClientSession() {
    boost::asio::io_service &io = ioService();
    return new ProxyDataSession(io, boost::asio::ip::tcp::socket(io));
}

void ProxySession::startProxying(std::shared_ptr<ProxyDataSession> session) {
    ProxyDataSession::start();
    session->start();
}

void ProxySession::start() {
    std::shared_ptr<ProxyDataSession> clientSession(createClientSession());
    auto self = std::dynamic_pointer_cast<ProxySession>(shared_from_this());

    m_outgoing->onConnect.connect_extended([self, clientSession](const boost::signals2::connection &c, bool connected) {
        if (connected) {
            self->setOther(clientSession);
            self->startProxying(clientSession);
        }

        // надо отключить слот чтоб self shared_ptr умер и отпустил объект
        // после этого время жизни ProxySession зависит от операций чтения/записи и m_outgoing сессии
        c.disconnect();
    });

    m_outgoing->setSession(clientSession);
    m_outgoing->connect(m_ip, m_port);
}

void ProxySession::setEndpoint(const std::string &ip, int port) {
    m_ip = ip;
    m_port = port;
}

std::shared_ptr<Client> ProxySession::client() const {
    return m_outgoing;
}
