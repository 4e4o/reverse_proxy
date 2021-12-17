#include "ServerInstance.hpp"
#include "Base/Network/Server.hpp"
#include "ServerSession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "Application.hpp"

#include <boost/asio/post.hpp>

#include <iostream>

ServerInstance::ServerInstance(boost::asio::io_service &io)
    : Instance(io),
      m_server(new Server<ServerSession>(io)) {
}

ServerInstance::~ServerInstance() {
}

void ServerInstance::start() {
    m_server->setSessionInit([this](Session* s) {
        ServerSession *p = static_cast<ServerSession*>(s);

        std::cout << "ServiceInstance::start new session " << p << std::endl;

        p->sessionTypeDefined.connect_extended([this](const boost::signals2::connection &c,
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

    m_control[s->serverId()] = s;
    sendSuccess(s);
    AAP->log("ServerInstance::onNewServiceControl %s %i", s.get(), s->serverId());
}

void ServerInstance::onServiceRequest(TSession s) {
    auto controlSession = m_control.find(s->serverId());

    if (controlSession == m_control.end()) {
        std::cout << "ServerInstance::onServiceRequest no server with id " <<
                     (int) s->serverId() << ", " << this << std::endl;
        return;
    }

    if (m_serviceRequests.find(s->serverId()) == m_serviceRequests.end())
        m_serviceRequests[s->serverId()] = TSessionQueue();

    m_serviceRequests[s->serverId()].push(s);
    controlSession->second->requestDataSession();

    AAP->log("ServerInstance::onServiceRequest %s %i", s.get(), s->serverId());
}

void ServerInstance::onServiceDataSession(TSession s) {
    auto r = m_serviceRequests.find(s->serverId());

    if (r == m_serviceRequests.end()) {
        std::cout << "ServerInstance::onServiceDataSession no server with id " <<
                     (int) s->serverId() << ", " << this << std::endl;
        return;
    }

    TSessionQueue& sq = r->second;
    TSession requester = sq.front();
    sq.pop();

    if (sq.empty())
        m_serviceRequests.erase(s->serverId());

    AAP->log("ServerInstance::onServiceDataSession %p %i", s.get(), s->serverId());

    s->onWriteDone.connect_extended([this, requester, s](const boost::signals2::connection &c) {
        c.disconnect();
        sendRequesterResponse(requester, s);
    });

    sendSuccess(s);
}

void ServerInstance::sendRequesterResponse(TSession r, TSession s) {
    r->onWriteDone.connect_extended([r, s](const boost::signals2::connection &c) {
        c.disconnect();
        r->setDataSession(s);
    });

    sendSuccess(r);
}

void ServerInstance::sendSuccess(TSession s) {
    uint8_t success = 1;
    s->writeAll((char*)&success, 1);
}
