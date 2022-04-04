#include "ServerProxySession.hpp"

TAwaitVoid ServerProxySession::work() {
    co_return co_await finishHandshake();
}
