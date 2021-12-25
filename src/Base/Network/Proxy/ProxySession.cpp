#include "ProxySession.hpp"
#include "Base/Network/Client.hpp"

using boost::signals2::connection;

ProxySession::~ProxySession() {
}

void ProxySession::setEndpoint(const std::string &ip, int port) {
    m_ip = ip;
    m_port = port;
}

ProxyDataSession* ProxySession::createClientSession() {
    return new ProxyDataSession(io());
}

void ProxySession::startImpl() {
    std::shared_ptr<Client> outgoint(new Client(io()));

    std::shared_ptr<ProxyDataSession> clientSession(createClientSession());
    auto self = shared_from_this<ProxySession>();

    outgoint->onConnect.connect_extended([self, clientSession](const connection &c, bool connected) {
        if (connected) {
            self->startProxying(clientSession);
        }

        // надо отключить слот чтоб self shared_ptr умер и отпустил объект
        // после этого время жизни ProxySession зависит от операций чтения/записи и m_outgoing сессии
        c.disconnect();
    });

    outgoint->setSession(clientSession);
    outgoint->connect(m_ip, m_port);
}
