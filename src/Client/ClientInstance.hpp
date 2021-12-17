#ifndef CLIENT_INSTANCE_HPP
#define CLIENT_INSTANCE_HPP

#include "Instance.hpp"

class IServer;

class ClientInstance : public Instance {
public:
    ClientInstance(boost::asio::io_service &io);

private:
    void start() override final;
    void stop() override final;

    std::shared_ptr<IServer> m_server;
};

#endif // CLIENT_INSTANCE_HPP
