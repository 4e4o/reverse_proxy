#ifndef SERVICE_CONTROL_SESSION_HPP
#define SERVICE_CONTROL_SESSION_HPP

#include "Protocol/ClientHandshake.hpp"

class ServiceControlSession : public ClientSession {
public:
    using ClientSession::ClientSession;

    boost::signals2::signal<void()> dataSessionRequest;

private:
    TAwaitVoid work() override final;
};

#endif // SERVICE_CONTROL_SESSION_HPP
