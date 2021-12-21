#ifndef SERVICE_CONTROL_SESSION_HPP
#define SERVICE_CONTROL_SESSION_HPP

#include "Base/Network/Session.hpp"
#include "Protocol/ClientHandshake.hpp"

class ServiceControlSession : public Session, public ClientHandshake {
public:
    using Session::Session;

    void start() override final;

    boost::signals2::signal<void()> dataSessionRequest;

private:
    void onHandshakeDone(TSession) override final;
    void onSSLInitDone();
    void doHandshake();
    void readRequest();
};

#endif // SERVICE_CONTROL_SESSION_HPP
