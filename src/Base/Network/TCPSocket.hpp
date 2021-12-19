#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio/ssl.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

class TCPSocket {
public:
    typedef typename boost::asio::io_service::executor_type executor_type;

    TCPSocket(boost::asio::io_service &io)
        : m_ssl(false),
          m_sslContext(boost::asio::ssl::context::tls),
          m_socket(io, m_sslContext) {
    }

    TCPSocket(TCPSocket&& socket)
        : TCPSocket(std::move(socket.m_socket.next_layer())) {
    }

    TCPSocket(boost::asio::ip::tcp::socket&& socket)
        : m_ssl(false),
          m_sslContext(boost::asio::ssl::context::tls),
          m_socket(std::move(socket), m_sslContext) {
    }

    template<typename... Args>
    void async_read_some(Args&&... args) {
        if (m_ssl)
            m_socket.async_read_some(std::forward<Args>(args)...);
        else
            m_socket.next_layer().async_read_some(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void async_write_some(Args&&... args) {
        if (m_ssl)
            m_socket.async_write_some(std::forward<Args>(args)...);
        else
            m_socket.next_layer().async_write_some(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void async_connect(Args&&... args) {
        m_socket.next_layer().async_connect(std::forward<Args>(args)...);
    }

    void close() {
        m_socket.next_layer().close();
    }

private:
    bool m_ssl;
    boost::asio::ssl::context m_sslContext;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_socket;
};

#endif // TCP_SOCKET_H
