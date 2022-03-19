#include "ClientInstance.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Server.hpp>
#include <Network/Session/Session.hpp>

#define WAIT_SECOND_SESSION_TIMEOUT     TSeconds(10)

ClientInstance::ClientInstance(boost::asio::io_context &io)
    : BaseClientInstance(io),
      m_server(new Server(io)) {
    m_server->stopped.connect(stopped);
}

void ClientInstance::start() {
    m_server->newSession.connect([this](TWSession ws) {
        debug_print(boost::format("Client server incoming session %1%") % ws.lock().get());
        ws.lock()->started.connect([this, ws] {
            auto s = ws.lock();
            s->wait(WAIT_SECOND_SESSION_TIMEOUT);
            proxy(ConnectionType::CLIENT, s);
        });
    });

    m_server->start(config()->listenIP(), config()->listenPort());
}

void ClientInstance::stop() {
    m_server->stop();
}
