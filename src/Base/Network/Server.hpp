#ifndef SERVER_H
#define SERVER_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/post.hpp>

#include <boost/signals2.hpp>

#include "IServer.hpp"

template<class Session>
class Server : public IServer {
public:
    using Acceptor = boost::asio::ip::tcp::acceptor;

    Server(boost::asio::io_service &io_service)
        : m_ioService(io_service),
          m_acceptor(m_ioService),
          m_socket(io_service),
          m_strand(io_service),
          m_stopped(false) {
    }

    void start(const std::string& ip, unsigned short port) override {
        m_acceptor = Acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
        doAccept();
    }

    void stop() override {
        post([this]() {
            if (m_stopped)
                return;

            m_stopped = true;
            m_acceptor.cancel();
            closeAllSessions();
        });
    }

protected:
    template <class Callable>
    void post(Callable&& c) {
        boost::asio::post(m_ioService, m_strand.wrap(std::move(c)));
    }

private:
    void doAccept() {
        m_acceptor.async_accept(m_socket,
                                m_strand.wrap([this](const boost::system::error_code& ec) {
            if (m_stopped)
                return;

            if (!ec) {
                auto session = std::make_shared<Session>(m_ioService, std::move(m_socket));
                auto initEvent = sessionInit();

                if (initEvent)
                    initEvent(session.get());

                std::weak_ptr<Session> weak_session = session;

                auto close_connection = closeAllSessions.connect_extended([weak_session](const boost::signals2::connection &c) {
                    c.disconnect();
                    auto ptr = weak_session.lock();

                    if (!ptr)
                        return;

                    ptr->close();
                });

                session->onDestroy.connect_extended([close_connection](const boost::signals2::connection &c) {
                    c.disconnect();
                    close_connection.disconnect();
                });

                session->start();
            }

            this->doAccept();
        }));
    }

    boost::signals2::signal<void ()> closeAllSessions;

    boost::asio::io_service &m_ioService;
    Acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::io_service::strand m_strand;
    bool m_stopped;
};

#endif // SERVER_H
