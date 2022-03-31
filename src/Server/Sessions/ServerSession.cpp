#include "ServerSession.hpp"
#include "Config/ConfigInstance.hpp"
#include "ServerProxySession.hpp"
#include "ServerControlSession.hpp"

#include <Network/Socket/TCP/SSLSocket.hpp>
#include <Misc/Debug.hpp>

ServerSession::ServerSession(Socket *s)
    : ServerHandshake(s) {
    debug_print_this("");
}

ServerSession::~ServerSession() {
    debug_print_this("");
}

TAwaitVoid ServerSession::prepare() {
    co_await ServerHandshake::prepare();
    SSLSocket* sock = socket<SSLSocket>();
    sock->setSSLParameters(config()->verifyHost(), config()->keysPath());
    sock->setClient(false);
}

TAwaitVoid ServerSession::work() {
    const ConnectionType type = co_await ServerHandshake::defineType();
    switch(type) {
    case ConnectionType::SERVICE_CLIENT_CONTROL: {
        auto control = mutate<ServerControlSession>(this);
        controlSession(control);
        control->start();
        break;
    }
    case ConnectionType::CLIENT: {
        clientSession(mutate<ServerProxySession>(this));
        break;
    }
    case ConnectionType::SERVICE_CLIENT_DATA: {
        dataSession(mutate<ServerProxySession>(this));
        break;
    }
    }
}
