#ifndef SERVER_CONTROL_SESSION_HPP
#define SERVER_CONTROL_SESSION_HPP

#include "Protocol/ServerHandshake.hpp"

class ServerSession;

class ServerControlSession : public ServerHandshake {
public:
    typedef std::shared_ptr<ServerControlSession> TSession;

    ServerControlSession(Socket*, ServerSession*);
    ~ServerControlSession();

    void requestDataSession();

    boost::signals2::signal<void()> registerSig;
    boost::signals2::signal<void()> unregisterSig;

private:
    TAwaitVoid work() override;
};

#endif // SERVER_CONTROL_SESSION_HPP
