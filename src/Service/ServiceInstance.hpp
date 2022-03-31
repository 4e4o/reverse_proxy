#ifndef SERVICE_INSTANCE_HPP
#define SERVICE_INSTANCE_HPP

#include "BaseClientInstance.hpp"

class ServiceControlSession;

class ServiceInstance : public BaseClientInstance {
public:
    ServiceInstance(boost::asio::io_context &io);
    ~ServiceInstance();

private:
    TAwaitVoid run() override;

    void startProxy();
};

#endif // SERVICE_INSTANCE_HPP
