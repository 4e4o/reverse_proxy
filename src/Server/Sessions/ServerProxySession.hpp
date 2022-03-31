#ifndef SERVER_PROXY_SESSION_HPP
#define SERVER_PROXY_SESSION_HPP

#include "Protocol/ServerHandshake.hpp"

class ServerSession;

class ServerProxySession : public ServerHandshake {
public:
    ServerProxySession(Socket*, ServerSession*);

private:
    TAwaitVoid work() override;
};

typedef std::shared_ptr<ServerProxySession> TServerProxySession;

#endif // SERVER_PROXY_SESSION_HPP
