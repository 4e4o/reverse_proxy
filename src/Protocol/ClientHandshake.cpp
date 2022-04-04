#include "ClientHandshake.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Session/Session.hpp>
#include <Network/Socket/TCP/SSLSocket.hpp>
#include <Network/Session/Operations/SessionReader.hpp>
#include <Network/Session/Operations/SessionWriter.hpp>

using namespace std::literals::chrono_literals;

#define DEFAULT_TIMEOUT 5s

ClientHandshake::ClientHandshake(Socket* s)
    : Session(s),
      m_skipSSLStrip(true),
      m_sessionType(ConnectionType::CLIENT) {
    setTimeout(Timeout::START, DEFAULT_TIMEOUT);
}

ClientHandshake::~ClientHandshake() {
}

void ClientHandshake::setSessionType(const ConnectionType &sessionType) {
    m_sessionType = sessionType;
}

void ClientHandshake::setSkipSSLStrip(bool skipSSLStrip) {
    m_skipSSLStrip = skipSSLStrip;
}

TAwaitVoid ClientHandshake::prepare() {
    co_await Session::prepare();
    SSLSocket* sock = socket<SSLSocket>();
    sock->setSSLParameters(config()->verifyHost(), config()->keysPath());
    sock->setClient(true);
}

TAwaitVoid ClientHandshake::work() {
    co_await Session::work();

    const uint8_t id = config()->serverId();
    CO_SES_WRITE(this, AS_BYTE(m_sessionType), id);

    co_await timeout(reader().all(1), DEFAULT_TIMEOUT);
    const bool success = reader().ptr()[0];

    debug_print_this(fmt("handshake success = %1%") % (int)success);

    if (!success) {
        throwGenericCoroutineError();
    }

    if (!m_skipSSLStrip) {
        // strip ssl layer if needed
        SSLSocket* sock = socket<SSLSocket>();
        sock->setSSL(config()->cryptData());
        CO_SES_WRITE(this, 1);
    }

    co_return;
}
