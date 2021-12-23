#include "ServiceInstance.hpp"
#include "Base/Network/Client.hpp"
#include "ServiceControlSession.hpp"
#include "Base/Network/Proxy/ProxyDataSession.hpp"
#include "Protocol/ClientProxySession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"

using boost::signals2::connection;

ServiceInstance::ServiceInstance(boost::asio::io_service &io)
    : Instance(io) {
}

ServiceInstance::~ServiceInstance() {
}

void ServiceInstance::start() {
    std::shared_ptr<ServiceControlSession> session(new ServiceControlSession(io()));
    std::shared_ptr<Client> client(new Client(io()));
    client->setSession(session);

    client->onConnect.connect_extended([this, session](const connection &c, bool connected) {
        c.disconnect();

        if (connected) {
            setControlSession(session);
        }
    });

    client->connect(APP->epIp(), APP->epPort());
}

void ServiceInstance::stop() {
    if (m_controlSession.get() != nullptr) {
        m_controlSession->close();
        m_controlSession.reset();
    }
}

void ServiceInstance::setControlSession(std::shared_ptr<ServiceControlSession> s) {
    AAP->log("ServiceInstance::setControlSession connected to remote server %p", this);

    m_controlSession = s;

    s->dataSessionRequest.connect([this]() {
        this->startDataChannels();
    });

    s->start();
}

void ServiceInstance::startDataChannels() {
    AAP->log("ServiceInstance::startDataChannels %p", this);

    std::shared_ptr<Session> session(new Session(io()));
    std::shared_ptr<Client> client(new Client(io()));
    client->setSession(session);

    client->onConnect.connect_extended([this, session](const connection &c, bool connected) {
        c.disconnect();

        if (connected) {
            startDataClient(session);
        }
    });

    client->connect(APP->ip(), APP->port());
}

void ServiceInstance::startDataClient(std::shared_ptr<Session> first) {
    AAP->log("ServiceInstance::startDataClient %p", this);

    std::shared_ptr<ClientProxySession> second(new ClientProxySession(io(), std::move(first->socket())));
    second->setEndpoint(APP->epIp(), APP->epPort());
    second->setSessionType(static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_DATA));
    second->start();
}
