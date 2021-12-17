#include "ClientHandshake.hpp"
#include "Base/Network/Session.hpp"
#include "Base/AApplication.h"

ClientHandshake::ClientHandshake(uint8_t type, uint8_t id)
    : m_sessionType(type), m_serverId(id) { }

void ClientHandshake::start(TSession s) {
    sendSessionType(s);
}

void ClientHandshake::sendSessionType(TSession s) {
    auto self = shared_from_this();

    s->onWriteDone.connect_extended([self, s](const boost::signals2::connection &c) {
        c.disconnect();
        self->onSessionTypeSended(s);
    });

    m_sendBuf = {m_sessionType, m_serverId};
    s->writeAll(m_sendBuf.data(), m_sendBuf.size());
}

void ClientHandshake::onSessionTypeSended(TSession s) {
    auto self = shared_from_this();

    s->onData.connect_extended([self, s](const boost::signals2::connection &c, const uint8_t *ptr, std::size_t) {
        c.disconnect();
        const uint8_t success = ptr[0];
        AAP->log("ClientHandshake success = %i", success);

        if (success) {
            self->onSuccessResponse();
        } else {
            s->close();
        }
    });

    s->readAll(1);
}
