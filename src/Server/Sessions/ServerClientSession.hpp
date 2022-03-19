#ifndef SERVER_CLIENT_SESSION_HPP
#define SERVER_CLIENT_SESSION_HPP

#include "ServerSessionChild.hpp"

class ServerClientSession : public ServerSessionChild {
public:
    typedef std::shared_ptr<ServerClientSession> TSession;

    ServerClientSession(ServerSession*);
    ~ServerClientSession();

    boost::signals2::signal<void()> cancelProxyRequest;

    void setProxySession(::TSession);

private:
    void startImpl() override final;
    void onHandshakeDone() override final;

    boost::optional<TWSession> m_other;
};

#endif // SERVER_CLIENT_SESSION_HPP
