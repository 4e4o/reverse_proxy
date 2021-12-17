#ifndef CLIENT_PROXY_SESSION_HPP
#define CLIENT_PROXY_SESSION_HPP

#include "Base/Network/Proxy/ProxySession.hpp"

class ClientProxySession : public ProxySession {
public:
    ClientProxySession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock);
    ~ClientProxySession();

    void setSessionType(const uint8_t &sessionType);

private:
    void startProxying(std::shared_ptr<ProxyDataSession> session) override final;

    uint8_t m_sessionType;
};

#endif // CLIENT_PROXY_SESSION_HPP
