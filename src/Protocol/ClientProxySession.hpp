#ifndef CLIENT_PROXY_SESSION_HPP
#define CLIENT_PROXY_SESSION_HPP

#include "Base/Network/Proxy/ProxySession.hpp"

class ClientProxySession : public ProxySession {
public:
    using ProxySession::ProxySession;
    ~ClientProxySession();

    void setSessionType(const uint8_t &sessionType);

private:
    void startProxying(std::shared_ptr<ProxyDataSession> session) override final;

    uint8_t m_sessionType;
};

#endif // CLIENT_PROXY_SESSION_HPP
