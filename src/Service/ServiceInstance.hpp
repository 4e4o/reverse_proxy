#ifndef SERVICE_INSTANCE_HPP
#define SERVICE_INSTANCE_HPP

#include <boost/signals2/signal.hpp>

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
    void startReconnectTimer();

    void start() override final;
    void stop() override final;

    static constexpr int RECONNECT_TIMEOUT_SEC = 10;

    bool m_stopped;
    boost::asio::steady_timer m_reconnectTimer;
    boost::signals2::signal<void ()> closeClients;
};

#endif // SERVICE_INSTANCE_HPP
