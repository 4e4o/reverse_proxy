#include "ServerInstance.hpp"
#include "Base/Network/Server.hpp"
#include "ServerSession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"

#include <boost/asio/post.hpp>

using boost::signals2::connection;

ServerInstance::ServerInstance(boost::asio::io_service &io)
    : Instance(io),
      m_server(new Server<ServerSession>(io)),
      m_success(1) {
}

ServerInstance::~ServerInstance() {
}

void ServerInstance::start() {
    m_server->setSessionInit([this](Session* s) {
        ServerSession *p = static_cast<ServerSession*>(s);

        AAP->log("ServiceInstance::start new session %p", p);

        p->sessionTypeDefined.connect_extended([this](const connection &c,
                                               std::shared_ptr<ServerSession> s) {
            c.disconnect();
            post([s, this]() {
                switch(s->type()) {
                case ConnectionType::CLIENT:
                    onServiceRequest(s);
                    break;
                case ConnectionType::SERVICE_CLIENT_CONTROL:
                    onNewServiceControl(s);
                    break;
                case ConnectionType::SERVICE_CLIENT_DATA:
                    onServiceDataSession(s);
                    break;
                }
            });
        });
    });

    m_server->start(APP->ip(), APP->port());
}

void ServerInstance::stop() {
    m_server->stop();
}

void ServerInstance::onNewServiceControl(TSession s) {
    auto ses = m_control.find(s->serverId());

    if (ses != m_control.end()) {
        ses->second->close();
        m_control.erase(s->serverId());
    }

    s->onData.connect_extended([](const connection &c, const uint8_t*, std::size_t) {
        c.disconnect();
        AAP->log("ServerInstance::onServiceDataSession onData SHOULD NEVER HAPPEN. FIX IT!!!!!");
    });

    s->onClose.connect_extended([this, s](const connection &c) {
        c.disconnect();
        m_control.erase(s->serverId());
        AAP->log("ServerInstance::onNewServiceControl onClose %p %i", s.get(), s->serverId());
    });

    m_control[s->serverId()] = s;
    sendSuccess(s);
    AAP->log("ServerInstance::onNewServiceControl %p %i", s.get(), s->serverId());

    // читаем что-нибудь чтоб определить дохлое соединение
    // никаких данных мы тут не получаем
    s->readSome();
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

    AAP->log("ServerInstance::onServiceRequest %p %i", s.get(), s->serverId());
}

void ServerInstance::onServiceDataSession(TSession s) {
    auto r = m_serviceRequests.find(s->serverId());

    if (r == m_serviceRequests.end()) {
        AAP->log("ServerInstance::onServiceDataSession no server with id id %i, %p", s->serverId(), this);
        return;
    }

    TSessionQueue& sq = r->second;
    TSession requester = sq.front();
    sq.pop();

    if (sq.empty())
        m_serviceRequests.erase(s->serverId());

    AAP->log("ServerInstance::onServiceDataSession %p %i", s.get(), s->serverId());

    s->onWriteDone.connect_extended([this, requester, s](const connection &c) {
        c.disconnect();
        sendRequesterResponse(requester, s);
    });

    sendSuccess(s);
}

void ServerInstance::sendRequesterResponse(TSession r, TSession s) {
    r->onWriteDone.connect_extended([r, s](const connection &c) {
        c.disconnect();
        r->setDataSession(s);
    });

    sendSuccess(r);
}

void ServerInstance::sendSuccess(TSession s) {
    s->writeAll(&m_success, 1);
}
