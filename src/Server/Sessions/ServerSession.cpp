#include "ServerSession.hpp"
#include "Config/ConfigInstance.hpp"
#include "ServerClientSession.hpp"
#include "ServerDataSession.hpp"
#include "ServerControlSession.hpp"

#include <Network/Socket/TCP/SSLSocket.hpp>
#include <Misc/Debug.hpp>

ServerSession::ServerSession(Socket *s)
    : Session(s),
      ServerHandshake(this) {
    debug_print(boost::format("ServerSession::ServerSession %1%") % this);
    starting.connect([this] {
        SSLSocket* sock = socket<SSLSocket>();
        sock->setSSLParameters(config()->verifyHost(), config()->keysPath());
        sock->setClient(false);
    });
}

ServerSession::~ServerSession() {
    debug_print(boost::format("ServerSession::~ServerSession %1%") % this);
}

void ServerSession::startImpl() {
    startHandshake();
}

void ServerSession::onClient() {
    std::shared_ptr<ServerClientSession> session(new ServerClientSession(this));
    clientSession(session);
    session->start();
    close();
}

void ServerSession::onClientData() {
    std::shared_ptr<ServerDataSession> session(new ServerDataSession(this));
    dataSession(session);
    session->start();
    close();
}

void ServerSession::onHandshakeDone() {
    if (type() != ConnectionType::SERVICE_CLIENT_CONTROL)
        return;

    std::shared_ptr<ServerControlSession> session(new ServerControlSession(this));
    controlSession(session);
    session->start();
    close();
}
