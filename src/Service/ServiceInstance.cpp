#include "ServiceInstance.hpp"
#include "ServiceControlSession.hpp"
#include "Config/ConfigInstance.hpp"
#include "Protocol/ClientSession.hpp"

#include <Misc/Debug.hpp>
#include <Misc/Lifecycle.hpp>
#include <Network/Client.hpp>

#define RECONNECT_TIMEOUT               TSeconds(10)
#define WAIT_SECOND_SESSION_TIMEOUT     TSeconds(10)

ServiceInstance::ServiceInstance(boost::asio::io_context &io)
    : BaseClientInstance(io),
      m_stopped(false) {
}

ServiceInstance::~ServiceInstance() {
}

void ServiceInstance::start() {
    post([this] {
        if (m_stopped)
            return;

        debug_print(boost::format("ServiceInstance::start %1%") % this);

        std::shared_ptr<Client> client(new Client(io(), RECONNECT_TIMEOUT));
        client->registerType<Session, ServiceControlSession, Socket*>();
        client->enableSSL();

        client->newSession.connect([this](TWSession ws) {
            auto session = std::static_pointer_cast<ServiceControlSession>(ws.lock());
            session->setConfig(config());
            session->dataSessionRequest.connect([this] {
                post([this] {
                    startDataChannels();
                });
            });
        });

        Lifecycle::connectTrack(stopped, client, &Client::stop);
        client->start(config()->remoteIP(), config()->remotePort());
    });
}

void ServiceInstance::stop() {
    post([this] {
        if (m_stopped)
            return;

        m_stopped = true;
        stopped();
    });
}

void ServiceInstance::startDataChannels() {
    debug_print(boost::format("ServiceInstance::startDataChannels %1%") % this);
    // сессия к локальному сервису, реконнекты не включаем
    std::shared_ptr<Client> client(new Client(io()));

    client->newSession.connect([this](TWSession ws) {
        auto s = ws.lock();
        s->started.connect([this, ws] {
            auto s = ws.lock();
            s->wait(WAIT_SECOND_SESSION_TIMEOUT);
            proxy(ConnectionType::SERVICE_CLIENT_DATA, s);
        });

        Lifecycle::connectTrack(stopped, s, &Session::close);
    });

    Lifecycle::connectTrack(stopped, client, &Client::stop);
    client->start(config()->listenIP(), config()->listenPort());
}
