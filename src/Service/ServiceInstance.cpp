#include "ServiceInstance.hpp"
#include "Base/Network/Client.hpp"
#include "ServiceControlSession.hpp"
#include "Base/Network/Proxy/ProxyDataSession.hpp"
#include "Protocol/ClientProxySession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"

#include <iostream>

ServiceInstance::ServiceInstance(boost::asio::io_service &io)
    : Instance(io) {
}

ServiceInstance::~ServiceInstance() {
}

void ServiceInstance::start() {
    std::shared_ptr<ServiceControlSession> s(new ServiceControlSession(io(), boost::asio::ip::tcp::socket(io())));
    std::shared_ptr<Client> cl(new Client(io()));
    cl->setSession(s);

    cl->onConnect.connect_extended([this, cl, s](const boost::signals2::connection &c, bool connected) {
        c.disconnect();

        if (connected) {
            setControlSession(s);
        }
    });

    cl->connect(APP->epIp(), APP->epPort());
}

void ServiceInstance::stop() {
    if (m_controlSession.get() != nullptr) {
        m_controlSession->close();
        m_controlSession.reset();
    }
}

void ServiceInstance::setControlSession(std::shared_ptr<ServiceControlSession> s) {
    std::cout << "ServiceInstance::setControlSession connected to remote server " << this << std::endl;

    m_controlSession = s;

    s->dataSessionRequest.connect([this]() {
        this->startDataChannels();
    });

    s->start();
}

void ServiceInstance::startDataChannels() {
    std::cout << "ServiceInstance::startDataChannels " << this << std::endl;

    std::shared_ptr<Session> s(new Session(io(), boost::asio::ip::tcp::socket(io())));
    std::shared_ptr<Client> cl(new Client(io()));
    cl->setSession(s);

    cl->onConnect.connect_extended([this, cl, s](const boost::signals2::connection &c, bool connected) {
        c.disconnect();

        if (connected) {
            startDataClient(s);
        }
    });

    cl->connect(APP->ip(), APP->port());
}

void ServiceInstance::startDataClient(std::shared_ptr<Session> s) {
    std::cout << "ServiceInstance::startDataClient " << this << std::endl;

    std::shared_ptr<ClientProxySession> ses(new ClientProxySession(io(), std::move(*s->socket())));
    ses->setEndpoint(APP->epIp(), APP->epPort());
    ses->setSessionType(static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_DATA));
    ses->start();
}
