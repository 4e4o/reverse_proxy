#include "ClientInstance.hpp"
#include "Config/ConfigInstance.hpp"

#include "Network/Session/Proxy/Proxy.hpp"
#include <Network/Server.hpp>
#include <Misc/Debug.hpp>

ClientInstance::ClientInstance(boost::asio::io_context &io)
    : BaseClientInstance(io, ConnectionType::CLIENT),
      m_server(new Server(io)) {
}

TAwaitVoid ClientInstance::run() {
    m_server->setHandler([this](TWSession ws) -> bool {
        auto s = ws.lock();
        debug_print_this(fmt("Client server incoming session %1%") % s.get());
        Proxy::TProxy proxy(new Proxy(io(), s, this));
        registerStop(proxy);
        proxy->start();
        return false;   // don't start session
    });

    m_server->start(config()->listenIP(), config()->listenPort());
    co_return;
}

TAwaitVoid ClientInstance::onStop() {
    m_server->stop();
    co_return;
}
