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

    const uint8_t toSend[2] = {m_sessionType, m_serverId};
    s->writeAll(reinterpret_cast<const char*>(toSend), sizeof(toSend));
}

void ClientHandshake::onSessionTypeSended(TSession s) {
    auto self = shared_from_this();

    s->onData.connect_extended([self, s](const boost::signals2::connection &c, char *ptr, std::size_t) {
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
