#include "ClientInstance.hpp"
#include "Base/Network/Server.hpp"
#include "Protocol/ClientProxySession.hpp"
#include "Application.hpp"
#include "Protocol/ConnectionType.hpp"

ClientInstance::ClientInstance(boost::asio::io_service &io)
    : Instance(io),
      m_server(new Server<ClientProxySession>(io)) {
}

void ClientInstance::start() {
    m_server->setSessionInit([](ClientProxySession* session) {
        session->setEndpoint(APP->epIp(), APP->epPort());
        session->setSessionType(static_cast<uint8_t>(ConnectionType::CLIENT));
    });

    m_server->start(APP->ip(), APP->port());
}

void ClientInstance::stop() {
    m_server->stop();
}
