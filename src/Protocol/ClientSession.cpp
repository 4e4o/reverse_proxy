#include "ClientSession.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Socket/TCP/SSLSocket.hpp>

ClientSession::ClientSession(Socket *s)
    : Session(s), ClientHandshake(this) {
    debug_print(boost::format("ClientSession::ClientSession %1%") % this);
    starting.connect([this] {
        SSLSocket* sock = socket<SSLSocket>();
        sock->setSSLParameters(config()->verifyHost(), config()->keysPath());
        sock->setClient(true);
    });
}

ClientSession::~ClientSession() {
    debug_print(boost::format("ClientSession::~ClientSession %1%") % this);
}

void ClientSession::startImpl() {
    startHandshake();
}

void ClientSession::onHandshakeDone() {
    handshakeDone();
}
