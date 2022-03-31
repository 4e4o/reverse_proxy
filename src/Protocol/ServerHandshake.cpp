#include "ServerHandshake.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Session/Session.hpp>
#include <Network/Socket/TCP/SSLSocket.hpp>
#include <Network/Session/Operations/SessionReader.hpp>
#include <Network/Session/Operations/SessionWriter.hpp>

using namespace std::literals::chrono_literals;

#define DEFAULT_TIMEOUT 5s

ServerHandshake::ServerHandshake(Socket* s, ServerHandshake* h)
    : ServerHandshake(s) {
    m_serverId = h->serverId();
    setConfig(h->config());
}

ServerHandshake::ServerHandshake(Socket* s)
    : Session(s) {
    setTimeout(Timeout::START, DEFAULT_TIMEOUT);
}

ServerHandshake::~ServerHandshake() {
}

uint8_t ServerHandshake::serverId() const {
    return m_serverId;
}

TAwaitConnectionType ServerHandshake::defineType() {
    co_await timeout(reader().all(2), DEFAULT_TIMEOUT);
    const uint8_t *ptr = reader().ptr();
    const ConnectionType type = static_cast<ConnectionType>(ptr[0]);
    m_serverId = ptr[1];

    switch(type) {
    case ConnectionType::SERVICE_CLIENT_CONTROL:
        CO_SES_WRITE(this, 1);
        break;
    case ConnectionType::CLIENT:
    case ConnectionType::SERVICE_CLIENT_DATA:
        break;
    default:
        debug_print_this("invalid session type");
        throwGenericCoroutineError();
    }

    co_return type;
}

TAwaitVoid ServerHandshake::finishHandshake() {
    CO_SES_WRITE(this, 1);

    // strip ssl layer if needed
    SSLSocket* sock = socket<SSLSocket>();
    sock->setSSL(config()->cryptData());

    co_await timeout(reader().all(1), DEFAULT_TIMEOUT);
    const uint8_t *ptr = reader().ptr();

    if (ptr[0] != 1) {
        throwGenericCoroutineError();
    }

    co_return;
}
