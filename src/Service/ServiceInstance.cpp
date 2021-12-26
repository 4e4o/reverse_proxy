#include "ServiceInstance.hpp"
#include "ServiceControlSession.hpp"
#include "Base/Network/Client.hpp"
#include "Base/Network/Proxy/ProxyDataSession.hpp"
#include "Protocol/ClientProxySession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"
#include "Config.hpp"

using boost::signals2::connection;

ServiceInstance::ServiceInstance(boost::asio::io_service &io)
    : Instance(io),
      m_stopped(false),
      m_reconnectTimer(io) {
}

ServiceInstance::~ServiceInstance() {
}

void ServiceInstance::startReconnectTimer() {
    if (m_stopped)
        return;

    m_reconnectTimer.expires_after(boost::asio::chrono::seconds(RECONNECT_TIMEOUT_SEC));
    m_reconnectTimer.async_wait([this](const boost::system::error_code& ec) {
        AAP->log("startReconnectTimer tick %i", ec);
        if (ec == boost::asio::error::operation_aborted)
            return;

        start();
    });
}

void ServiceInstance::start() {
    post([this] () {
        AAP->log("ServiceInstance::start %p", this);

        std::shared_ptr<ServiceControlSession> session(new ServiceControlSession(io()));
        std::shared_ptr<Client> client(new Client(io()));
        client->setSession(session);

        session->onDestroy.connect_extended([this](const connection &c) {
            c.disconnect();

            post([this]() {
                startReconnectTimer();
            });
        });

        client->onConnect.connect_extended([this, session](const connection &c, bool connected) {
            c.disconnect();

            if (connected) {
                post([this, session]() {
                    setControlSession(session);
                });
            }
        });

        client->connect(CONFIG.remoteIP, CONFIG.remotePort);
    });
}

void ServiceInstance::stop() {
    post([this]() {
        if (m_stopped)
            return;

        m_stopped = true;

        try {
            m_reconnectTimer.cancel();
        } catch(...) { }

        closeClients();
    });
}

void ServiceInstance::setControlSession(std::shared_ptr<ServiceControlSession> s) {
    AAP->log("ServiceInstance::setControlSession connected to remote server %p", this);

    s->dataSessionRequest.connect([this]() {
        post([this] {
            startDataChannels();
        });
    });

    closeClients.connect(decltype(closeClients)::slot_type(
                             &ServiceControlSession::close, s.get()).track_foreign(s));

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
            post([this, session]{
                startDataClient(session);
            });
        }
    });

    client->connect(CONFIG.listenIP, CONFIG.listenPort);
}

void ServiceInstance::startDataClient(std::shared_ptr<Session> first) {
    AAP->log("ServiceInstance::startDataClient %p", this);

    std::shared_ptr<ClientProxySession> second(new ClientProxySession(io(), std::move(first->socket())));
    second->setEndpoint(CONFIG.remoteIP, CONFIG.remotePort);
    second->setSessionType(static_cast<uint8_t>(ConnectionType::SERVICE_CLIENT_DATA));
    second->start();

    closeClients.connect(decltype(closeClients)::slot_type(
                             &ClientProxySession::close, second.get()).track_foreign(second));
}
