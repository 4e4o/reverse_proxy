#include "Client.hpp"
#include "Base/Network/Session.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <iostream>

Client::Client(boost::asio::io_service &io_service)
    : m_strand(io_service),
      m_connectTimer(io_service) {
    std::cout << "Client::Client " << this << std::endl;
}

Client::~Client() {
    std::cout << "Client::~Client " << this << std::endl;
}

void Client::setSession(std::shared_ptr<Session> c) {
    m_session = c;
}

std::shared_ptr<Session> Client::session() const {
    return m_session;
}

void Client::connect(std::string ip, unsigned short port) {
    if (m_session == nullptr)
        return;

    using namespace boost::asio::ip;
    using boost::system::error_code;

    tcp::endpoint endpoint(address_v4::from_string(ip), port);
    auto self = shared_from_this();

    m_session->socket()->async_connect(endpoint,
                                       m_strand.wrap([self](const error_code& ec) {
        self->m_connectTimer.cancel();

        if (ec) {
            self->onConnect(false);
        } else {
            self->onConnect(true);
        }
    }));

    m_connectTimer.expires_after(boost::asio::chrono::seconds(CONNECT_TIMEOUT_SEC));
    m_connectTimer.async_wait(m_strand.wrap([self](const error_code& ec) {
        if (ec == boost::asio::error::operation_aborted)
            return;

        self->m_session->close();
        self->onConnect(false);
    }));
}
