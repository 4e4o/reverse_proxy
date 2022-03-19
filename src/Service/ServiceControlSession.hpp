#ifndef SERVICE_CONTROL_SESSION_HPP
#define SERVICE_CONTROL_SESSION_HPP

#include "Protocol/ClientSession.hpp"

class ServiceControlSession : public ClientSession {
public:
    using ClientSession::ClientSession;

    boost::signals2::signal<void()> dataSessionRequest;

private:
    void startImpl() override final;
    void receiveRequests();
};

#endif // SERVICE_CONTROL_SESSION_HPP
