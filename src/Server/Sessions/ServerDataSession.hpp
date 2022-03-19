#ifndef SERVER_DATA_SESSION_HPP
#define SERVER_DATA_SESSION_HPP

#include "ServerSessionChild.hpp"

class ServerDataSession : public ServerSessionChild {
public:
    typedef std::shared_ptr<ServerDataSession> TSession;

    ServerDataSession(ServerSession*);
    ~ServerDataSession();

    void setProxySession(::TSession);

private:
    void startImpl() override final;
    void onHandshakeDone() override final;

    boost::optional<TWSession> m_other;
};

#endif // SERVER_DATA_SESSION_HPP
