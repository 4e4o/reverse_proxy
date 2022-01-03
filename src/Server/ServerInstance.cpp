#include "ServerInstance.hpp"
#include "Base/Network/Server.hpp"
#include "ServerSession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"
#include "Config.hpp"

using boost::signals2::connection;

ServerInstance::ServerInstance(boost::asio::io_service &io)
    : Instance(io),
      m_server(new Server<ServerSession>(io)) {
}

ServerInstance::~ServerInstance() {
}

void ServerInstance::start() {
    m_server->setSessionInit([this](ServerSession* session) {
        AAP->log("ServiceInstance::start new session %p", session);

        session->onControlSession.connect_extended([this](const connection &c,
                                                   std::shared_ptr<ServerSession> s) {
            c.disconnect();
            post([s, this]() {
                onNewServiceControl(s);
            });
        });

        session->onClientSession.connect_extended([this](const connection &c,
                                                  std::shared_ptr<ServerSession> s) {
            c.disconnect();
            post([s, this]() {
                onServiceRequest(s);
            });
        });

        session->onClientDataSession.connect_extended([this](const connection &c,
                                                      std::shared_ptr<ServerSession> s) {
            c.disconnect();
            post([s, this]() {
                onServiceDataSession(s);
            });
        });
    });

    m_server->start(CONFIG.listenIP, CONFIG.listenPort);
}

void ServerInstance::stop() {
    m_server->stop();
}

void ServerInstance::onNewServiceControl(TSession s) {
    auto ses = m_control.find(s->serverId());

    if (ses != m_control.end()) {
        AAP->log("ServerInstance::onNewServiceControl closing existing control session %p %i", ses->second.get(), s->serverId());
        ses->second->close();
        m_control.erase(s->serverId());
    }

    s->onClose.connect_extended([this, s](const connection &c) {
        c.disconnect();
        post([s, this]() {
            auto ses = m_control.find(s->serverId());

            if (ses == m_control.end())
                return;

            const bool canErase = ses->second.get() == s.get();

            if (canErase)
                m_control.erase(s->serverId());

            AAP->log("ServerInstance::onNewServiceControl onClose %p %i %i", s.get(), s->serverId(), canErase);
        });
    });

    m_control[s->serverId()] = s;

    AAP->log("ServerInstance::onNewServiceControl %p %i", s.get(), s->serverId());
}

void ServerInstance::onServiceRequest(TSession s) {
    auto controlSession = m_control.find(s->serverId());

    if (controlSession == m_control.end()) {
        AAP->log("ServerInstance::onServiceRequest no server with id %i, %p", s->serverId(), this);
        return;
    }

    if (m_serviceRequests.find(s->serverId()) == m_serviceRequests.end())
        m_serviceRequests[s->serverId()] = TSessionQueue();

    m_serviceRequests[s->serverId()].push(s);
    controlSession->second->requestDataSession();

    AAP->log("ServerInstance::onServiceRequest %p %i %i", s.get(), s->serverId(), m_serviceRequests[s->serverId()].size());
}

void ServerInstance::onServiceDataSession(TSession s) {
    auto r = m_serviceRequests.find(s->serverId());

    if (r == m_serviceRequests.end()) {
        AAP->log("ServerInstance::onServiceDataSession no server with id %i, %p", s->serverId(), this);
        return;
    }

    TSessionQueue& sq = r->second;
    ServerSession::TSession requester = std::dynamic_pointer_cast<ServerSession>(sq.front());
    sq.pop();

    if (sq.empty())
        m_serviceRequests.erase(s->serverId());

    AAP->log("ServerInstance::onServiceDataSession %p %p %i %i", s.get(), requester.get(), s->serverId(), sq.size());

    requester->setDataSession(s);
}
