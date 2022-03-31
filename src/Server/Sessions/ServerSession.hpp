#ifndef SERVER_SESSION_HPP
#define SERVER_SESSION_HPP

#include "Protocol/ServerHandshake.hpp"

class ServerControlSession;
class ServerProxySession;

/**
 * @brief The ServerSession class
 *
 * Начальная сессия для сервера, определяет по хендшейку тип сессии и создаёт новую
 *
 */

class ServerSession : public ServerHandshake {
public:
    typedef std::shared_ptr<ServerSession> TSession;

    ServerSession(Socket*);
    ~ServerSession();

    boost::signals2::signal<void(std::weak_ptr<ServerControlSession>)> controlSession;
    boost::signals2::signal<void(std::weak_ptr<ServerProxySession>)> clientSession;
    boost::signals2::signal<void(std::weak_ptr<ServerProxySession>)> dataSession;

private:
    TAwaitVoid prepare() override;
    TAwaitVoid work() override final;
};

#endif // SERVER_SESSION_HPP
