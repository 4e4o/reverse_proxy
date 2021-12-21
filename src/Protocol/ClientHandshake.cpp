#include "ClientHandshake.hpp"
#include "Base/Network/Session.hpp"
#include "Base/AApplication.h"

void ClientHandshake::startHandshake(TSession s, uint8_t type, uint8_t serverId) {
    m_serverId = serverId;
    m_sessionType = type;
    sendSessionType(s);
}

void ClientHandshake::sendSessionType(TSession s) {
    s->onWriteDone.connect_extended([this, s](const boost::signals2::connection &c) {
        c.disconnect();
        onSessionTypeSended(s);
    });

    m_sendBuf = {m_sessionType, m_serverId};
    s->writeAll(m_sendBuf.data(), m_sendBuf.size());
}

void ClientHandshake::onSessionTypeSended(TSession s) {
    s->onData.connect_extended([this, s](const boost::signals2::connection &c, const uint8_t *ptr, std::size_t) {
        c.disconnect();
        const uint8_t success = ptr[0];
        AAP->log("ClientHandshake success = %i", success);

        if (success) {
            onHandshakeDone(s);
        } else {
            s->close();
        }
    });

    s->readAll(1);
}
