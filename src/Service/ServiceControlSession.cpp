#include "ServiceControlSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Network/Session/Operations/SessionReader.hpp>

TAwaitVoid ServiceControlSession::work() {
    setSessionType(ConnectionType::SERVICE_CLIENT_CONTROL);
    setSkipSSLStrip(true);

    co_await ClientSession::work();
    const uint8_t *ptr = reader().ptr();

    while(running()) {
        co_await reader().all(2);
        const ConnectionType type = static_cast<ConnectionType>(ptr[0]);

        if ((type != ConnectionType::SERVICE_CLIENT_DATA) || (ptr[1] != config()->serverId()))
            continue;

        dataSessionRequest();
    }
}
