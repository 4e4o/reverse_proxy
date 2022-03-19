#ifndef CLIENT_INSTANCE_HPP
#define CLIENT_INSTANCE_HPP

#include "BaseClientInstance.hpp"

class Server;

class ClientInstance : public BaseClientInstance {
public:
    ClientInstance(boost::asio::io_context &io);

private:
    void start() override final;
    void stop() override final;

    std::shared_ptr<Server> m_server;
};

#endif // CLIENT_INSTANCE_HPP
