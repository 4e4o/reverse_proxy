#ifndef SERVICE_CONTROL_SESSION_HPP
#define SERVICE_CONTROL_SESSION_HPP

#include "Base/Network/Session.hpp"

class ClientHandshake;

class ServiceControlSession : public Session {
public:
    ServiceControlSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock);

    void start() override final;

    boost::signals2::signal<void()> dataSessionRequest;

private:
    void readRequest();
};

#endif // SERVICE_CONTROL_SESSION_HPP
