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

    void setEndpoint(const std::string&, int port);

protected:
    void startImpl() override;
    virtual ProxyDataSession* createClientSession();

private:
    std::shared_ptr<Client> m_outgoing;
    std::string m_ip;
    int m_port;
};

#endif // PROXY_SESSION_H
