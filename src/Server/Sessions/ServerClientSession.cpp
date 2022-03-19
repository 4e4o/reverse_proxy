#include "ServerClientSession.hpp"
#include "ServerDataSession.hpp"

#include <Misc/Debug.hpp>

#define DATA_CLIENT_TIMEOUT     TSeconds(10)
#define WAIT_PROXYING_TIMEOUT   TSeconds(10)

ServerClientSession::ServerClientSession(ServerSession *s)
    : ServerSessionChild(s) {
    debug_print(boost::format("ServerClientSession::ServerClientSession %1%") % this);
    closing.connect([this] {
        // если не пришла сессия то отменяем запрос
        if (!m_other.has_value()) {
            cancelProxyRequest();
        }
    });
}

ServerClientSession::~ServerClientSession() {
    debug_print(boost::format("ServerClientSession::~ServerClientSession %1%") % this);
}

void ServerClientSession::startImpl() {
    wait(DATA_CLIENT_TIMEOUT);
}

void ServerClientSession::setProxySession(::TSession s) {
    post<true>([this, s] {
        STRAND_ASSERT(this);
        debug_print(boost::format("ServerClientSession::setProxySessionImpl %1%") % this);
        m_other = s;
        finishHandshake();
    });
}

void ServerClientSession::onHandshakeDone() {    
    auto other = m_other->lock();

    if (other == nullptr)
        return;

    wait(WAIT_PROXYING_TIMEOUT);
    auto otherDataSession = std::static_pointer_cast<ServerDataSession>(other);
    otherDataSession->setProxySession(shared_from_this<ServerClientSession>());
}
