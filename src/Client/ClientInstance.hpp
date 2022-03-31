#ifndef CLIENT_INSTANCE_HPP
#define CLIENT_INSTANCE_HPP

#include "BaseClientInstance.hpp"

#include <Network/ServerForward.hpp>

class ClientInstance : public BaseClientInstance {
public:
    ClientInstance(boost::asio::io_context &io);

private:
    TAwaitVoid run() override;
    TAwaitVoid onStop() override;

    TServer m_server;
};

#endif // CLIENT_INSTANCE_HPP
