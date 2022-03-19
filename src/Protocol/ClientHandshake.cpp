#include "ClientHandshake.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Session/Session.hpp>
#include <Network/Socket/TCP/SSLSocket.hpp>
#include <Network/Session/Operations/SessionReader.hpp>
#include <Network/Session/Operations/SessionWriter.hpp>

#define START_TIMEOUT       TSeconds(5)
#define RESPONSE_TIMEOUT    TSeconds(6)

ClientHandshake::ClientHandshake(Session* s)
    : m_skipSSLStrip(true),
      m_sessionType(ConnectionType::CLIENT),
      m_session(s) {
    m_session->setTimeout(Session::Timeouts::START, START_TIMEOUT);
}

ClientHandshake::~ClientHandshake() {
}

void ClientHandshake::setSessionType(const ConnectionType &sessionType) {
    m_sessionType = sessionType;
}

void ClientHandshake::setSkipSSLStrip(bool skipSSLStrip) {
    m_skipSSLStrip = skipSSLStrip;
}

void ClientHandshake::startHandshake() {
    sendSessionType();
}

void ClientHandshake::sendSessionType() {
    const uint8_t id = config()->serverId();
    m_session->writer()([this] {
        onSessionTypeSended();
    }, { BYTE(m_sessionType), id });
}

void ClientHandshake::onSessionTypeSended() {
    m_session->setTimeout(Session::Timeouts::READ, RESPONSE_TIMEOUT);
    m_session->reader()([this](const uint8_t *ptr) {
        m_session->setTimeout(Session::Timeouts::READ);
        const bool success = ptr[0];
        debug_print(boost::format("ClientHandshake success = %1%") % (int)success);

        if (success)
            finalStep();
    }, 1);
}

void ClientHandshake::finalStep() {
    if (m_skipSSLStrip) {
        onHandshakeDone();
        return;
    }

    // strip ssl layer if needed
    SSLSocket* sock = m_session->socket<SSLSocket>();
    sock->setSSL(config()->cryptData());

    m_session->writer()([this] { onHandshakeDone(); }, {1});
}
