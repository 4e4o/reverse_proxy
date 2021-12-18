#include "ServiceControlSession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Protocol/ClientHandshake.hpp"
#include "Application.hpp"

void ServiceControlSession::start() {
    auto self = std::dynamic_pointer_cast<ServiceControlSession>(shared_from_this());
    std::shared_ptr<ClientHandshake> h(new ClientHandshake(static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_CONTROL), APP->serverId()));

    h->onSuccessResponse.connect_extended([self](const boost::signals2::connection &c) {
        c.disconnect();
        self->readRequest();
    });

    h->start(self);
}

void ServiceControlSession::readRequest() {
    auto self = std::dynamic_pointer_cast<ServiceControlSession>(shared_from_this());

    self->onData.connect_extended([self](const boost::signals2::connection &c, const uint8_t *ptr, std::size_t) {
        c.disconnect();

        if (static_cast<ConnectionType>(ptr[0]) != ConnectionType::SERVICE_CLIENT_DATA)
            return;

        if (ptr[1] != APP->serverId())
            return;

        self->dataSessionRequest();
        self->readRequest();
    });

    self->readAll(2);
}
