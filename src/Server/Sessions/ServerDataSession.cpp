#include "ServerDataSession.hpp"

#include <Misc/Debug.hpp>
#include <Network/Session/Proxy/ProxyDataSession.hpp>

#define WAIT_OTHER_TIMEOUT  TSeconds(10)

ServerDataSession::ServerDataSession(ServerSession *s)
    : ServerSessionChild(s) {
    debug_print(boost::format("ServerDataSession::ServerDataSession %1%") % this);
}

ServerDataSession::~ServerDataSession() {
    debug_print(boost::format("ServerDataSession::~ServerDataSession %1%") % this);
}

void ServerDataSession::startImpl() {
    wait(WAIT_OTHER_TIMEOUT);
}

void ServerDataSession::setProxySession(::TSession s) {
    post<true>([this, s] {
        STRAND_ASSERT(this);
        debug_print(boost::format("ServerDataSession::setProxySessionImpl %1%") % this);
        m_other = s;
        finishHandshake();
    });
}

void ServerDataSession::onHandshakeDone() {
    auto other = m_other->lock();

    if (other == nullptr)
        return;

    ProxyDataSession::proxy(shared_from_this<Session>(), other);
}
