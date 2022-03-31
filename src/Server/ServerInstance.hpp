#ifndef SERVER_INSTANCE_HPP
#define SERVER_INSTANCE_HPP

#include "Instance.hpp"

#include <Network/Session/Proxy/AsyncProxyProvider.hpp>

class Server;
class ServerControlSession;
class ServerProxySession;

class ServerInstance : public Instance, public AsyncProxyProviderDelegate {
public:
    ServerInstance(boost::asio::io_context &);
    ~ServerInstance();

private:
    TAwaitVoid run() override;
    TAwaitVoid onStop() override;

    TSessionClass classify(TSession) override;
    TAwaitBool startAsyncRequest(TSessionClass) override;

    typedef std::shared_ptr<ServerProxySession> TProxySession;
    typedef std::shared_ptr<ServerControlSession> TControlSession;

    void addControl(TControlSession);
    void removeControl(TControlSession);

    std::shared_ptr<Server> m_server;
    std::shared_ptr<AsyncProxyProvider> m_provider;
    std::unordered_map<uint8_t, TControlSession> m_controls;
};

#endif // SERVER_INSTANCE_HPP
