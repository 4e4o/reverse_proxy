#include "ServerProxySession.hpp"
#include "ServerSession.hpp"

ServerProxySession::ServerProxySession(Socket* sock, ServerSession* s)
    : ServerHandshake(sock, s) {
}

TAwaitVoid ServerProxySession::work() {
    co_await finishHandshake();
    co_return;
}
