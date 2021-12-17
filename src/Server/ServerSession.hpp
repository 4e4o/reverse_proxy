#ifndef SERVER_SESSION_HPP
#define SERVER_SESSION_HPP

#include "Base/Network/Proxy/ProxyDataSession.hpp"
#include "Protocol/ConnectionType.hpp"

class ServerSession : public ProxyDataSession {
public:
    typedef std::shared_ptr<ServerSession> TSession;

    ServerSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock);
    ~ServerSession();

    void start() override final;

    boost::signals2::signal<void(TSession)> sessionTypeDefined;

    ConnectionType type() const;
    uint8_t serverId() const;

    void requestDataSession();
    void setDataSession(TSession);

private:
    void startClient();
    void startServiceData();
    void startServiceControl();

    ConnectionType m_type;
    uint8_t m_serverId;
};

#endif // SERVER_SESSION_HPP
