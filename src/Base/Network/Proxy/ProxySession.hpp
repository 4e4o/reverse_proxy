#ifndef PROXY_SESSION_H
#define PROXY_SESSION_H

#include "ProxyDataSession.hpp"

// ProxyDataSession сессия с клиентом

class ProxySession : public ProxyDataSession {
public:
    using ProxyDataSession::ProxyDataSession;
    ~ProxySession();

    void setEndpoint(const std::string&, int port);

protected:
    void startImpl() override;
    virtual ProxyDataSession* createClientSession();

private:
    std::string m_ip;
    int m_port;
};

#endif // PROXY_SESSION_H
