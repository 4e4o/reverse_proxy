#include "ServerInstance.hpp"
#include "Sessions/ServerSession.hpp"
#include "Sessions/ServerProxySession.hpp"
#include "Sessions/ServerControlSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Network/Server.hpp>
#include <Network/Session/Proxy/Proxy.hpp>
#include <Misc/Debug.hpp>

ServerInstance::ServerInstance(boost::asio::io_context &io)
    : Instance(io),
      m_server(new Server(io)),
      m_provider(new AsyncProxyProvider(io, this)) {
    m_server->registerType<Session, ServerSession, Socket*>();
    m_server->enableSSL();
}

ServerInstance::~ServerInstance() {
}

TAwaitVoid ServerInstance::run() {
    m_server->setHandler([this](TWSession ws) -> bool {
        auto session = std::static_pointer_cast<ServerSession>(ws.lock());
        debug_print_this(fmt("new session %1%") % session.get());
        session->setConfig(config());

        session->controlSession.connect([this](std::weak_ptr<ServerControlSession> ws) {
            ws.lock()->registerSig.connect([this, ws] {
                post([this, s = ws.lock()] {
                    addControl(s);
                });
                ws.lock()->unregisterSig.connect([this, ws] {
                    post([this, s = ws.lock()] {
                        removeControl(s);
                    });
                });
            });
        });

        session->clientSession.connect([this](std::weak_ptr<ServerProxySession> ws) {
            post([this, s = ws.lock()] {
                Proxy::TProxy proxy(new Proxy(io(), s, m_provider.get()));
                registerStop(proxy);
                proxy->start();
            });
        });

        session->dataSession.connect([this](std::weak_ptr<ServerProxySession> ws) {
            post([this, s = ws.lock()] {
                m_provider->asyncResponse(s);
            });
        });

        return true;
    });

    m_server->start(config()->listenIP(), config()->listenPort());
    co_return;
}

TAwaitVoid ServerInstance::onStop() {
    m_server->stop();
    co_return;
}

void ServerInstance::addControl(TControlSession s) {
    STRAND_ASSERT(this);
    auto ses = m_controls.find(s->serverId());

    if (ses != m_controls.end()) {
        debug_print_this(fmt("closing existing control session %1% %2%") % ses->second.get() % (int)s->serverId());
        ses->second->stop();
        m_controls.erase(s->serverId());
    }

    m_controls[s->serverId()] = s;
    registerStop(s);
    debug_print_this(fmt("%1% %2% %3%") % s.get() % (int)s->serverId() % m_controls.size());
}

void ServerInstance::removeControl(TControlSession s) {
    STRAND_ASSERT(this);
    auto ses = m_controls.find(s->serverId());

    if (ses == m_controls.end())
        return;

    const bool canErase = ses->second.get() == s.get();

    if (canErase)
        m_controls.erase(s->serverId());

    debug_print_this(fmt("%1% %2% %3% %4%") % s.get() % (int)s->serverId() % (int)canErase % m_controls.size());
}

ServerInstance::TSessionClass ServerInstance::classify(TSession s) {
    TServerProxySession session = std::static_pointer_cast<ServerProxySession>(s);
    return session->serverId();
}

TAwaitBool ServerInstance::startAsyncRequest(TSessionClass cl) {
    return spawn([this, cl]() -> TAwaitBool {
        STRAND_ASSERT(this);
        auto controlSession = m_controls.find(cl);

        if (controlSession == m_controls.end()) {
            debug_print_this(fmt("no control session with serverId = %1%") % cl);
            co_return false;
        }

        controlSession->second->requestDataSession();
        co_return true;
    }, use_awaitable);
}
