#include "ServerSession.hpp"
#include "Base/AApplication.h"

ServerSession::~ServerSession() {
}

ConnectionType ServerSession::type() const {
    return m_type;
}

uint8_t ServerSession::serverId() const {
    return m_serverId;
}

void ServerSession::startImpl() {
    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());
    startSSL(false, "client1", [self]() {
        self->readClientType();
    });
}

void ServerSession::readClientType() {
    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());

    onData.connect_extended([self](const boost::signals2::connection &c, const uint8_t *ptr, std::size_t) {
        c.disconnect();

        self->m_dataRequestsCount = 0;
        self->m_type = static_cast<ConnectionType>(ptr[0]);
        self->m_serverId = ptr[1];

        switch(self->m_type) {
        case ConnectionType::CLIENT:
        case ConnectionType::SERVICE_CLIENT_CONTROL:
        case ConnectionType::SERVICE_CLIENT_DATA:
            break;
        default:
            AAP->log("ServerSession::onData invalid type %p", self.get());
            return;
        }

        self->m_requestBuffer = {static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_DATA), self->m_serverId};
        self->sessionTypeDefined(self);
    });

    self->readAll(2);
}

// -------------- client

void ServerSession::setDataSession(TSession s) {
    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());
    auto other = std::dynamic_pointer_cast<ServerSession>(s);

    post([self, other]() {
        AAP->log("ServerSession::setDataSession %p %p", other.get(), self.get());

        // remove encryption layer
        self->socket().setSSL(false);
        other->socket().setSSL(false);

        self->ProxyDataSession::startProxying(other, false);
        other->ProxyDataSession::startProxying(self, false);
    });
}
// --------------

// -------------- service control

void ServerSession::requestDataSession() {
    auto self = std::dynamic_pointer_cast<ServerSession>(shared_from_this());

    post([self]() {
        self->onWriteDone.connect_extended([self](const boost::signals2::connection &c) {
            c.disconnect();
            self->m_dataRequestsCount--;

            if (self->m_dataRequestsCount > 0)
                self->requestDataSession();
        });

        AAP->log("ServerSession::requestDataSession %p", self.get());
        self->m_dataRequestsCount++;
        self->writeAll(self->m_requestBuffer.data(), self->m_requestBuffer.size());
    });
}
// --------------
