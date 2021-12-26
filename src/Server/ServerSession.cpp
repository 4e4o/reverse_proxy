#include "ServerSession.hpp"
#include "Base/AApplication.h"

using boost::signals2::connection;

ServerSession::~ServerSession() {
}

ConnectionType ServerSession::type() const {
    return m_type;
}

uint8_t ServerSession::serverId() const {
    return m_serverId;
}

void ServerSession::startImpl() {
    auto self = shared_from_this<ServerSession>();
    socket().setSSLParameters("client1");
    startSSL(false, [self]() {
        self->readClientType();
    });
}

void ServerSession::readClientType() {
    auto self = shared_from_this<ServerSession>();

    onData.connect_extended([self](const connection &c, const uint8_t *ptr, std::size_t) {
        c.disconnect();

        self->m_dataRequestsCount = 0;
        self->m_type = static_cast<ConnectionType>(ptr[0]);
        self->m_serverId = ptr[1];
        self->m_requestBuffer = {static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_DATA), self->m_serverId};
        self->m_requestWriting = false;

        switch(self->m_type) {
        case ConnectionType::CLIENT:
            self->onClient();
            break;
        case ConnectionType::SERVICE_CLIENT_CONTROL:
            self->onControl();
            break;
        case ConnectionType::SERVICE_CLIENT_DATA:
            self->onClientData();
            break;
        default:
            AAP->log("ServerSession::onData invalid type %p", self.get());
            return;
        }
    });

    self->readAll(2);
}

void ServerSession::onControl() {
    onData.connect_extended([](const connection &c, const uint8_t*, std::size_t) {
        c.disconnect();
        AAP->log("ServerSession::onControl onData SHOULD NEVER HAPPEN. FIX IT!!!!!");
    });

    // читаем что-нибудь чтоб определить дохлое соединение
    // никаких данных мы тут не получаем
    readSome();

    // шлём 1 и ждём отсылки
    auto self = shared_from_this<ServerSession>();

    onWriteDone.connect_extended([self](const connection &c) {
        c.disconnect();
        self->onControlSession(self);
    });

    sendSuccess();
}

void ServerSession::onClient() {
    auto self = shared_from_this<ServerSession>();
    onClientSession(self);
}

void ServerSession::onClientData() {
    auto self = shared_from_this<ServerSession>();
    onClientDataSession(self);
}

void ServerSession::finishClientHandshake(TEvent e) {
    auto self = shared_from_this<ServerSession>();

    onWriteDone.connect_extended([self, e](const connection &c) {
        c.disconnect();
        self->socket().setSSL(false);
        self->onData.connect_extended([e](const connection &c, const uint8_t* ptr, std::size_t) {
            c.disconnect();
            if (ptr[0] == 1)
                e();
        });
        self->readAll(1);
    });

    sendSuccess();
}

void ServerSession::setDataSession(TSession s) {
    auto self = shared_from_this<ServerSession>();
    auto other = std::dynamic_pointer_cast<ServerSession>(s);

    post([self, other]() {
        AAP->log("ServerSession::setDataSession %p %p", other.get(), self.get());

        self->finishClientHandshake([other, self]() {
            other->finishClientHandshake([other, self]() {
                self->ProxyDataSession::startProxying(other, false);
                other->ProxyDataSession::startProxying(self, false);
            });
        });
    });
}

void ServerSession::processDataSessionRequest() {
    if ((m_dataRequestsCount <= 0) || m_requestWriting)
        return;

    auto self = shared_from_this<ServerSession>();

    onWriteDone.connect_extended([self](const connection &c) {
        c.disconnect();
        self->m_requestWriting = false;
        self->post([self]() {
            self->processDataSessionRequest();
        });
    });

    m_dataRequestsCount--;
    m_requestWriting = true;
    writeAll(m_requestBuffer.data(), m_requestBuffer.size());
}

void ServerSession::requestDataSession() {
    auto self = shared_from_this<ServerSession>();
    post([self]() {
        self->m_dataRequestsCount++;
        AAP->log("ServerSession::requestDataSession %p %i", self.get(), self->m_dataRequestsCount);
        self->processDataSessionRequest();
    });
}

void ServerSession::sendSuccess() {
    m_success = 1;
    writeAll(&m_success, 1);
}
