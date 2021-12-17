#include "ServerSession.hpp"
#include "Base/AApplication.h"

#include <iostream>

ServerSession::ServerSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock)
    : ProxyDataSession(io_service, std::move(sock)) {
    std::cout << "ServerSession::ServerSession " << this << std::endl;
}

ServerSession::~ServerSession() {
    std::cout << "ServerSession::~ServerSession " << this << std::endl;
}

ConnectionType ServerSession::type() const {
    return m_type;
}

uint8_t ServerSession::serverId() const {
    return m_serverId;
}

void ServerSession::start() {
    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());

    onData.connect_extended([self](const boost::signals2::connection &c, char *ptr, std::size_t) {
        c.disconnect();
        self->m_type = static_cast<ConnectionType>(ptr[0]);
        self->m_serverId = static_cast<uint8_t>(ptr[1]);

        switch(self->m_type) {
        case ConnectionType::CLIENT:
        case ConnectionType::SERVICE_CLIENT_CONTROL:
        case ConnectionType::SERVICE_CLIENT_DATA:
            break;
        default:
            std::cout << "ServerSession::onData invalid type " << self.get() << std::endl;
            return;
        }

        self->sessionTypeDefined(self);

        switch(self->m_type) {
        case ConnectionType::CLIENT:
            self->startClient();
            break;
        case ConnectionType::SERVICE_CLIENT_CONTROL:
            self->startServiceControl();
            break;
        case ConnectionType::SERVICE_CLIENT_DATA:
            self->startServiceData();
            break;
        }
    });

    self->readAll(2);
}

// -------------- client
void ServerSession::startClient() {
    // TODO Timeout
//    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());
//    serviceRequest(self);
}

void ServerSession::setDataSession(TSession s) {
    AAP->log("ServerSession::setDataSession %p %p", s.get(), this);

    setOther(s);
    ProxyDataSession::start();    
    s->ProxyDataSession::start();
}
// --------------

// -------------- service control
void ServerSession::startServiceControl() {
    // TODO keep alive set
//    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());
//    newServiceControl(self);
//    start protocol
}

void ServerSession::requestDataSession() {
    const uint8_t t = static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_DATA);
    const uint8_t toSend[2] = {t, m_serverId};
    writeAll(reinterpret_cast<const char*>(toSend), sizeof(toSend));
}
// --------------

void ServerSession::startServiceData() {
    // nothing todo, delete?
//    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());
//    newServiceData(self);
}
