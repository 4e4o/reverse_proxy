#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio/ssl.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

class TCPSocket {
public:
    typedef typename boost::asio::io_service::executor_type executor_type;
    typedef std::function<void(bool)> TInitResult;

    TCPSocket(boost::asio::io_service &io);
    TCPSocket(boost::asio::ip::tcp::socket&& socket);
    TCPSocket(TCPSocket&& socket);
    virtual ~TCPSocket();

    void initSSL(boost::asio::ssl::stream_base::handshake_type,
                 const std::string& verifyHost, TInitResult);
    void setSSL(bool);

    template<typename... Args>
    void async_read_some(Args&&... args) {
        if (m_ssl)
            m_socket->async_read_some(std::forward<Args>(args)...);
        else
            m_socket->next_layer().async_read_some(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void async_write_some(Args&&... args) {
        if (m_ssl)
            m_socket->async_write_some(std::forward<Args>(args)...);
        else
            m_socket->next_layer().async_write_some(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void async_connect(Args&&... args) {
        m_socket->next_layer().async_connect(std::forward<Args>(args)...);
    }

    void init();
    void close();

private:
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> TSocket;
    typedef boost::asio::ssl::context TContext;

    bool m_ssl;
    std::unique_ptr<TContext> m_sslContext;
    std::unique_ptr<TSocket> m_socket;
};

#endif // TCP_SOCKET_H
