#include "BaseClientInstance.hpp"
#include "Protocol/ClientSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Misc/Lifecycle.hpp>
#include <Network/Client.hpp>
#include <Network/Session/Proxy/ProxyDataSession.hpp>

BaseClientInstance::BaseClientInstance(boost::asio::io_context &io)
    : Instance(io) {
}

void BaseClientInstance::proxy(ConnectionType type, std::shared_ptr<Session> first) {
    std::shared_ptr<Client> client(new Client(io()));
    client->registerType<Session, ClientSession, Socket*>();
    client->enableSSL();

    client->newSession.connect([this, type, first](TWSession ws) {
        auto second = std::static_pointer_cast<ClientSession>(ws.lock());

        second->setConfig(config());
        second->setSkipSSLStrip(false);
        second->setSessionType(type);

        second->handshakeDone.connect([this, first, ws] {
            auto s1 = ProxyDataSession::proxy(first, ws.lock());
            Lifecycle::connectTrack(stopped, s1, &Session::close);
        });

        Lifecycle::connectTrack(stopped, second, &Session::close);
    });

    Lifecycle::connectTrack(stopped, client, &Client::stop);
    client->start(config()->remoteIP(), config()->remotePort());
}
