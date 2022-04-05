#include "ServiceInstance.hpp"
#include "ServiceControlSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Client.hpp>
#include <Network/Session/Proxy/Proxy.hpp>

using namespace boost::asio;
using namespace std::literals::chrono_literals;

#define RECONNECT_TIMEOUT   10s

ServiceInstance::ServiceInstance(io_context &io)
    : BaseClientInstance(io, ConnectionType::SERVICE_CLIENT_DATA) {
}

ServiceInstance::~ServiceInstance() {
}

TAwaitVoid ServiceInstance::run() {    
    debug_print_this("");

    TClient client(new Client(io(), RECONNECT_TIMEOUT));
    client->registerType<Session, ServiceControlSession, Socket*>();
    client->enableSSL();

    client->newSession.connect([this](TWSession ws) {
        auto session = std::static_pointer_cast<ServiceControlSession>(ws.lock());
        session->setConfig(config());
        session->dataSessionRequest.connect([this] {
            post([this] {
                startProxy();
            });
        });
    });

    co_await client->co_start(use_awaitable, config()->remoteIP(), config()->remotePort());
    co_return;
}

void ServiceInstance::startProxy() {
    debug_print_this("");

    // сессия к локальному сервису, реконнекты не включаем
    TClient client(new Client(io()));

    client->newSession.connect([this](TWSession ws) {
        debug_print_this("new local service session");
        Proxy::TProxy proxy(new Proxy(io(), ws.lock(), this));
        registerStop(proxy);
        proxy->start();
    });

    registerStop(client);
    client->start(config()->listenIP(), config()->listenPort());
}
