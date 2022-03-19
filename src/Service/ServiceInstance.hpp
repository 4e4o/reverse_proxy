#ifndef SERVICE_INSTANCE_HPP
#define SERVICE_INSTANCE_HPP

#include "BaseClientInstance.hpp"

class ServiceControlSession;

class ServiceInstance : public BaseClientInstance {
public:
    ServiceInstance(boost::asio::io_context &io);
    ~ServiceInstance();

private:
    void startDataChannels();

    void start() override final;
    void stop() override final;

    bool m_stopped;
};

#endif // SERVICE_INSTANCE_HPP
