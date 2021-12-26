#ifndef SERVER_H
#define SERVER_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/post.hpp>

#include <boost/signals2.hpp>

#include "Base/Network/TCPSocket.hpp"

template<class Session>
class Server {
public:
    using Acceptor = boost::asio::ip::tcp::acceptor;

    Server(boost::asio::io_service &io)
        : m_io(io),
          m_acceptor(io),
          m_socket(io),
          m_strand(io),
          m_stopped(false) {
    }

    void start(const std::string& ip, unsigned short port) {
        m_acceptor = Acceptor(m_io, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
        doAccept();
    }

    void stop() {
        post([this]() {
            if (m_stopped)
                return;

            m_stopped = true;
            m_acceptor.cancel();
            closeAllSessions();
        });
    }

    typedef std::function<void(Session*)> TSessionInitEvent;

    void setSessionInit(TSessionInitEvent&& e) {
        m_sessionInit = std::move(e);
    }

protected:
    template <class Callable>
    void post(Callable&& c) {
        boost::asio::post(m_io, m_strand.wrap(std::move(c)));
    }

private:
    void doAccept() {
        m_acceptor.async_accept(m_socket,
                                m_strand.wrap([this](const boost::system::error_code& ec) {
            if (m_stopped)
                return;

            if (!ec) {
                auto session = std::make_shared<Session>(m_io, std::move(TCPSocket(std::move(m_socket))));

                if (m_sessionInit)
                    m_sessionInit(session.get());

                closeAllSessions.connect(typename decltype(closeAllSessions)::slot_type(
                                             &Session::close, session.get()).track_foreign(session));

                session->start();
            }

            this->doAccept();
        }));
    }

    boost::signals2::signal<void()> closeAllSessions;
    boost::asio::io_service &m_io;
    Acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::io_service::strand m_strand;
    bool m_stopped;
    TSessionInitEvent m_sessionInit;
};

#endif // SERVER_H
