#include "ServerControlSession.hpp"
#include "Protocol/ConnectionType.hpp"

#include <Network/Session/Operations/SessionReader.hpp>
#include <Network/Session/Operations/SessionWriter.hpp>
#include <Misc/Debug.hpp>

ServerControlSession::ServerControlSession(ServerSession* s)
    : ServerSessionChild(s) {
    debug_print(boost::format("ServerControlSession::ServerControlSession %1%") % this);
}

ServerControlSession::~ServerControlSession() {
    debug_print(boost::format("ServerControlSession::~ServerControlSession %1%") % this);
}

void ServerControlSession::startImpl() {
    // читаем что-нибудь чтоб определить дохлое соединение
    // никаких данных мы тут не получаем
    reader()([](const uint8_t*, std::size_t) {
        AAP->log("ServerControlSession::reader data for control session SHOULD NEVER HAPPEN. FIX IT!!!!!");
    });
}

void ServerControlSession::requestDataSession() {
    post<true>([this] {
        STRAND_ASSERT(this);
        debug_print(boost::format("ServerControlSession::requestDataSession %1%") % this);
        const uint8_t id = serverId();
        const uint8_t type = BYTE(ConnectionType::SERVICE_CLIENT_DATA);
        writer()([]{}, {type, id});
    });
}
