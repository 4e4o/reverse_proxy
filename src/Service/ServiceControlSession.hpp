#ifndef SERVICE_CONTROL_SESSION_HPP
#define SERVICE_CONTROL_SESSION_HPP

#include "Base/Network/Session.hpp"

class ClientHandshake;

class ServiceControlSession : public Session {
public:
    using Session::Session;

    void start() override final;

    boost::signals2::signal<void()> dataSessionRequest;

private:
    void readRequest();
};

#endif // SERVICE_CONTROL_SESSION_HPP
