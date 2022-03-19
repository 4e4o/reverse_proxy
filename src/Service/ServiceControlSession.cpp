#include "ServiceControlSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Network/Session/Operations/SessionReader.hpp>

void ServiceControlSession::startImpl() {
    setSessionType(ConnectionType::SERVICE_CLIENT_CONTROL);
    setSkipSSLStrip(true);
    handshakeDone.connect([this] { receiveRequests(); });
    ClientSession::startImpl();
}

void ServiceControlSession::receiveRequests() {
    reader()([this](const uint8_t *ptr) {
        reader().repeat();
        const ConnectionType type = static_cast<ConnectionType>(ptr[0]);

        if ((type != ConnectionType::SERVICE_CLIENT_DATA) || (ptr[1] != config()->serverId()))
            return;

        dataSessionRequest();
    }, 2);
}
