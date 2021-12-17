#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/signals2.hpp>
#include <boost/asio/strand.hpp>

class Session;

class Client : public std::enable_shared_from_this<Client> {
public:
    Client(boost::asio::io_service &io_service);
    virtual ~Client();

    void setSession(std::shared_ptr<Session>);
    std::shared_ptr<Session> session() const;

    void connect(std::string ip, unsigned short port);

    boost::signals2::signal<void (bool)> onConnect;

private:
    static constexpr int CONNECT_TIMEOUT_SEC = 30;

    boost::asio::io_service::strand m_strand;
    boost::asio::steady_timer m_connectTimer;
    std::shared_ptr<Session> m_session;
};

#endif // CLIENT_H
