#include "ServiceControlSession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"

void ServiceControlSession::startImpl() {
    auto self = Session::shared_from_this<ServiceControlSession>();
    startSSL(true, "server1", [self]() {
        self->onSSLInitDone();
    });
}

void ServiceControlSession::onSSLInitDone() {
    auto self = Session::shared_from_this<ServiceControlSession>();
    startHandshake(self, static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_CONTROL), APP->serverId());
}

void ServiceControlSession::onHandshakeDone(TSession) {
    readRequest();
}

void ServiceControlSession::readRequest() {
    auto self = Session::shared_from_this<ServiceControlSession>();

    self->onData.connect_extended([self](const boost::signals2::connection &c, const uint8_t *ptr, std::size_t) {
        c.disconnect();

        if ((static_cast<ConnectionType>(ptr[0]) != ConnectionType::SERVICE_CLIENT_DATA) || (ptr[1] != APP->serverId())) {
            self->close();
            return;
        }

        self->dataSessionRequest();
        self->readRequest();
    });

    self->readAll(2);
}
