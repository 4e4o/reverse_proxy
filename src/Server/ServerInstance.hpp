#ifndef SERVER_INSTANCE_HPP
#define SERVER_INSTANCE_HPP

#include "Instance.hpp"

#include <map>
#include <queue>

class ServerSession;
template<class Session> class Server;

class ServerInstance : public Instance {
public:
    ServerInstance(boost::asio::io_service &io_service);
    ~ServerInstance();

    void start() override final;
    void stop() override final;

private:
    typedef std::shared_ptr<ServerSession> TSession;
    typedef std::queue<TSession> TSessionQueue;

    void onNewServiceControl(TSession);
    void onServiceRequest(TSession);
    void onServiceDataSession(TSession);
    void sendRequesterResponse(TSession, TSession);
    void sendSuccess(TSession);

    std::shared_ptr<Server<ServerSession>> m_server;
    std::map<uint8_t, TSession> m_control;
    std::map<uint8_t, TSessionQueue> m_serviceRequests;
    const uint8_t m_success;
};

#endif // SERVER_INSTANCE_HPP
