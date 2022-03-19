#include "ServerHandshake.hpp"
#include "Config/ConfigInstance.hpp"

#include <Misc/Debug.hpp>
#include <Network/Session/Session.hpp>
#include <Network/Socket/TCP/SSLSocket.hpp>
#include <Network/Session/Operations/SessionReader.hpp>
#include <Network/Session/Operations/SessionWriter.hpp>

#define START_TIMEOUT               TSeconds(5)
#define FIRST_CMD_RECEIVE_TIMEOUT   TSeconds(3)
#define FINISH_RECEIVE_TIMEOUT      TSeconds(5)

using boost::signals2::connection;

ServerHandshake::ServerHandshake(Session* s, ServerHandshake* h)
    : ServerHandshake(s) {
    m_type = h->type();
    m_serverId = h->serverId();
    setConfig(h->config());
}

ServerHandshake::ServerHandshake(Session* s)
    : m_session(s) {
    m_session->setTimeout(Session::Timeouts::START, START_TIMEOUT);
}

ServerHandshake::~ServerHandshake() {
}

void ServerHandshake::startHandshake() {
    readClientType();
}

ConnectionType ServerHandshake::type() const {
    return m_type;
}

uint8_t ServerHandshake::serverId() const {
    return m_serverId;
}

void ServerHandshake::readClientType() {
    m_session->setTimeout(Session::Timeouts::READ, FIRST_CMD_RECEIVE_TIMEOUT);
    m_session->reader()([this](const uint8_t* ptr) {
        m_session->setTimeout(Session::Timeouts::READ);
        m_type = static_cast<ConnectionType>(ptr[0]);
        m_serverId = ptr[1];

        switch(m_type) {
        case ConnectionType::SERVICE_CLIENT_CONTROL:
            onControl();
            break;
        case ConnectionType::CLIENT:
            onClient();
            break;
        case ConnectionType::SERVICE_CLIENT_DATA:
            onClientData();
            break;
        default:
            debug_print(boost::format("ServerHandshake::onData invalid type %1%") % this);
            return;
        }
    }, 2);
}

void ServerHandshake::onControl() {
    m_session->writer()([this] { onHandshakeDone(); }, {1});
}

void ServerHandshake::finishHandshake() {
    m_session->writer()([this] {
        // strip ssl layer if needed
        SSLSocket* sock = m_session->socket<SSLSocket>();
        // FIXME CRASH inside cryptData
        sock->setSSL(config()->cryptData());

        m_session->setTimeout(Session::Timeouts::READ, FINISH_RECEIVE_TIMEOUT);
        m_session->reader()([this](const uint8_t* ptr) {
            m_session->setTimeout(Session::Timeouts::READ);
            if (ptr[0] == 1) {
                onHandshakeDone();
            }
        }, 1);
    }, {1});
}
