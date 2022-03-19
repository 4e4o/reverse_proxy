#include "ServerInstance.hpp"
#include "Sessions/ServerSession.hpp"
#include "Sessions/ServerClientSession.hpp"
#include "Sessions/ServerControlSession.hpp"
#include "Sessions/ServerDataSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Misc/Lifecycle.hpp>
#include <Network/Server.hpp>
#include <Network/Socket/Socket.hpp>

using boost::signals2::connection;

ServerInstance::ServerInstance(boost::asio::io_context &io)
    : Instance(io),
      m_server(new Server(io)) {
    m_server->registerType<Session, ServerSession, Socket*>();
    m_server->enableSSL();
}

ServerInstance::~ServerInstance() {
}

void ServerInstance::start() {
    m_server->newSession.connect([this](TWSession ws) {
        auto session = std::static_pointer_cast<ServerSession>(ws.lock());
        debug_print(boost::format("ServiceInstance::start new session %1%") % session);
        session->setConfig(config());

        session->controlSession.connect([this](std::weak_ptr<ServerControlSession> ws) {
            ws.lock()->started.connect([this, ws] {
                post([this, s = ws.lock()] {
                    addControl(s);
                });
                ws.lock()->closing.connect([this, ws] {
                    post([this, s = ws.lock()] {
                        removeControl(s);
                    });
                });
            });
        });

        session->clientSession.connect([this](std::weak_ptr<ServerClientSession> ws) {
            ws.lock()->started.connect([this, ws] {
                post([this, s = ws.lock()] {
                    addRequester(s);
                });
                ws.lock()->cancelProxyRequest.connect([this, ws] {
                    post([this, s = ws.lock()] {
                        removeRequester(s);
                    });
                });
            });
        });

        session->dataSession.connect([this](std::weak_ptr<ServerDataSession> ws) {
            ws.lock()->started.connect([this, ws] {
                post([this, s = ws.lock()] {
                    onDataSession(s);
                });
            });
        });
    });

    m_server->start(config()->listenIP(), config()->listenPort());
}

void ServerInstance::stop() {
    m_server->stop();
}

void ServerInstance::addControl(TControlSession s) {
    STRAND_ASSERT(this);
    auto ses = m_controls.find(s->serverId());

    if (ses != m_controls.end()) {
        debug_print(boost::format("ServerInstance::addControl closing existing control session %1% %2%") % ses->second.get() % (int)s->serverId());
        ses->second->close();
        m_controls.erase(s->serverId());
    }

    m_controls[s->serverId()] = s;
    Lifecycle::connectTrack(m_server->stopped, s, &Session::close);
    debug_print(boost::format("ServerInstance::addControl %1% %2%") % s.get() % (int)s->serverId());
}

void ServerInstance::removeControl(TControlSession s) {
    STRAND_ASSERT(this);
    auto ses = m_controls.find(s->serverId());

    if (ses == m_controls.end())
        return;

    const bool canErase = ses->second.get() == s.get();

    if (canErase)
        m_controls.erase(s->serverId());

    debug_print(boost::format("ServerInstance::removeControl %1% %2% %3%") % s.get() % (int)s->serverId() % canErase);
}

void ServerInstance::addRequester(TClientSession s) {
    STRAND_ASSERT(this);
    auto controlSession = m_controls.find(s->serverId());

    if (controlSession == m_controls.end()) {
        debug_print(boost::format("ServerInstance::addRequester no control session with serverId = %1%, %2%") % (int)s->serverId() % s.get());
        s->close();
        return;
    }

    if (m_requesters.find(s->serverId()) == m_requesters.end())
        m_requesters[s->serverId()] = TClientSessions();

    m_requesters[s->serverId()].push_back(s);
    controlSession->second->requestDataSession();

    debug_print(boost::format("ServerInstance::addRequester %1% %2% %3%") % s.get() % (int)s->serverId() % m_requesters[s->serverId()].size());
}

void ServerInstance::removeRequester(TClientSession s) {
    STRAND_ASSERT(this);
    auto r = m_requesters.find(s->serverId());

    if (r == m_requesters.end()) {
        debug_print(boost::format("ServerInstance::removeRequester no requesters with serverId = %1%, %2%") % (int)s->serverId() % s.get());
        return;
    }

    TClientSessions& sessions = r->second;

    auto it = std::find_if(sessions.begin(), sessions.end(),
                           [s](const TClientSession& i) { return i.get() == s.get(); });

    if (it == sessions.end()) {
        debug_print(boost::format("ServerInstance::removeRequester can't find session %1%, %2%") % (int)s->serverId() % this);
        return;
    }

    sessions.erase(it);

    if (sessions.empty())
        m_requesters.erase(s->serverId());

    debug_print(boost::format("ServerInstance::removeRequester requester erased serverId = %1%, %2%") % (int)s->serverId() % this);
}

void ServerInstance::onDataSession(TDataSession s) {
    STRAND_ASSERT(this);
    auto r = m_requesters.find(s->serverId());

    if (r == m_requesters.end()) {
        debug_print(boost::format("ServerInstance::onServiceDataSession no requester with serverId = %1%, %2%") % (int)s->serverId() % s.get());
        s->close();
        return;
    }

    TClientSessions& sessions = r->second;
    // если тут пустая очередь значит мы удалям где-то реквестера и не удаляем пустую очередь если она стала таковой.
    assert(!sessions.empty());
    TClientSession requester = sessions.front();
    sessions.pop_front();

    debug_print(boost::format("ServerInstance::onServiceDataSession %1% %2% %3% %4%") % s.get() % requester.get() % (int)s->serverId() % sessions.size());

    if (sessions.empty())
        m_requesters.erase(s->serverId());

    requester->setProxySession(s);
}
