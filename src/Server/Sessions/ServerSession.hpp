#ifndef SERVER_SESSION_HPP
#define SERVER_SESSION_HPP

#include "Protocol/ServerHandshake.hpp"

#include <Network/Session/Session.hpp>

class ServerControlSession;
class ServerClientSession;
class ServerDataSession;

/**
 * @brief The ServerSession class
 *
 * Начальная сессия для сервера, определяет по хендшейку тип сессии и создаёт новую
 *
 */

class ServerSession : public Session, public ServerHandshake {
public:
    typedef std::shared_ptr<ServerSession> TSession;

    ServerSession(Socket*);
    ~ServerSession();

    boost::signals2::signal<void(std::weak_ptr<ServerControlSession>)> controlSession;
    boost::signals2::signal<void(std::weak_ptr<ServerClientSession>)> clientSession;
    boost::signals2::signal<void(std::weak_ptr<ServerDataSession>)> dataSession;

private:
    void startImpl() override final;
    void onClient() override final;
    void onClientData() override final;
    void onHandshakeDone() override final;
};

#endif // SERVER_SESSION_HPP
