#ifndef SERVICE_INSTANCE_HPP
#define SERVICE_INSTANCE_HPP

#include "Instance.hpp"

class Session;
class ServiceControlSession;

class ServiceInstance : public Instance {
public:
    ServiceInstance(boost::asio::io_service &io);
    ~ServiceInstance();

private:
    void setControlSession(std::shared_ptr<ServiceControlSession>);
    void startDataChannels();
    void startDataClient(std::shared_ptr<Session>);

    void start() override final;
    void stop() override final;

    std::shared_ptr<ServiceControlSession> m_controlSession;
};

#endif // SERVICE_INSTANCE_HPP
