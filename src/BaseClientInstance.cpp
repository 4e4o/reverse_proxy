#include "BaseClientInstance.hpp"
#include "Protocol/ClientHandshake.hpp"
#include "Config/ConfigInstance.hpp"

#include <Network/Client.hpp>
#include <Misc/Debug.hpp>

using namespace boost::asio;

BaseClientInstance::BaseClientInstance(io_context &io, ConnectionType t)
    : Instance(io),
      m_type(t) {
}

TAwaitSession BaseClientInstance::get(ISessionRequester*) {
    return spawn([this]() -> TAwaitSession {
        TClient client(new Client(io()));
        client->registerType<Session, ClientSession, Socket*>();
        client->enableSSL();
        auto s = co_await client->co_start(use_awaitable, config()->remoteIP(), config()->remotePort());
        auto session = std::static_pointer_cast<ClientSession>(s);
        session->setConfig(config());
        session->setSkipSSLStrip(false);
        session->setSessionType(m_type);
        co_return s;
    }, use_awaitable);
}
