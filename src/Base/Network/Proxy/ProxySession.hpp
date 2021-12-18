#ifndef PROXY_SESSION_H
#define PROXY_SESSION_H

#include "Base/Network/Client.hpp"
#include "ProxyDataSession.hpp"

class Client;

// ProxyDataSession сессия с клиентом

class ProxySession : public ProxyDataSession {
public:
    using ProxyDataSession::ProxyDataSession;
    ~ProxySession();

    void start() override;
    void setEndpoint(const std::string&, int port);

protected:
    virtual ProxyDataSession* createClientSession();
    virtual void startProxying(std::shared_ptr<ProxyDataSession> session);

private:
    std::shared_ptr<Client> m_outgoing;
    std::string m_ip;
    int m_port;
};

#endif // PROXY_SESSION_H
