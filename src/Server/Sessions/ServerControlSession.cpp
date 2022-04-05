#include "ServerControlSession.hpp"
#include "Protocol/ConnectionType.hpp"
#include "ServerSession.hpp"

#include <AApplication.h>
#include <Network/Session/Operations/SessionReader.hpp>
#include <Network/Session/Operations/SessionWriter.hpp>
#include <Misc/ScopeGuard.hpp>
#include <Misc/Debug.hpp>

ServerControlSession::ServerControlSession(Socket* sock, ServerSession* s)
    : ServerHandshake(sock, s) {
    debug_print_this("");
}

ServerControlSession::~ServerControlSession() {
    debug_print_this("");
}

TAwaitVoid ServerControlSession::work() {
    registerSig();
    ScopeGuard un([this] { unregisterSig(); });
    co_await reader().some();
    AAP_LOG("ServerControlSession::reader data for control session SHOULD NEVER HAPPEN. FIX IT!!!!!");
    throwGenericCoroutineError();
}

void ServerControlSession::requestDataSession() {
    spawn<true>([this]() -> TAwaitVoid {
        STRAND_ASSERT(this);
        debug_print_this("");
        const uint8_t id = serverId();
        const uint8_t type = AS_BYTE(ConnectionType::SERVICE_CLIENT_DATA);
        CO_SES_WRITE(this, type, id);
    });
}
