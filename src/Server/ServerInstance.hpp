#ifndef SERVER_INSTANCE_HPP
#define SERVER_INSTANCE_HPP

#include "Instance.hpp"

#include <map>
#include <list>

class Server;
class ServerControlSession;
class ServerClientSession;
class ServerDataSession;

class ServerInstance : public Instance {
public:
    ServerInstance(boost::asio::io_context &);
    ~ServerInstance();

    void start() override final;
    void stop() override final;

private:
    typedef std::shared_ptr<ServerClientSession> TClientSession;
    typedef std::shared_ptr<ServerControlSession> TControlSession;
    typedef std::shared_ptr<ServerDataSession> TDataSession;

    typedef std::list<TClientSession> TClientSessions;

    void addControl(TControlSession);
    void removeControl(TControlSession);

    void addRequester(TClientSession);
    void removeRequester(TClientSession);

    void onDataSession(TDataSession);

    std::shared_ptr<Server> m_server;
    std::map<uint8_t, TControlSession> m_controls;
    std::map<uint8_t, TClientSessions> m_requesters;
};

#endif // SERVER_INSTANCE_HPP
