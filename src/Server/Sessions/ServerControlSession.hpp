#ifndef SERVER_CONTROL_SESSION_HPP
#define SERVER_CONTROL_SESSION_HPP

#include "ServerSessionChild.hpp"

class ServerControlSession : public ServerSessionChild {
public:
    typedef std::shared_ptr<ServerControlSession> TSession;

    ServerControlSession(ServerSession*);
    ~ServerControlSession();

    void requestDataSession();

private:
    void startImpl() override final;
};

#endif // SERVER_CONTROL_SESSION_HPP
