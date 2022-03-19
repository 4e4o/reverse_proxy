#ifndef CLIENT_SESSION_HPP
#define CLIENT_SESSION_HPP

#include <Network/Session/Session.hpp>

#include "Protocol/ClientHandshake.hpp"

// Сессия клиента к прокси серверу

class ClientSession : public Session, public ClientHandshake {
public:
    ClientSession(Socket*);
    ~ClientSession();

    boost::signals2::signal<void ()> handshakeDone;

protected:
    void startImpl() override;

private:
    void onHandshakeDone() override final;
};

#endif // CLIENT_SESSION_HPP
