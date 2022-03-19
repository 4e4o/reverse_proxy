#ifndef SERVER_SESSION_CHILD_HPP
#define SERVER_SESSION_CHILD_HPP

#include "Protocol/ServerHandshake.hpp"

#include <Network/Session/Session.hpp>

/**
 * @brief The ServerClientSession class
 *
 * Отпочковавшаяся от ServerSession сессия
 *
 */

class ServerSession;

class ServerSessionChild : public Session, public ServerHandshake {
public:
    ServerSessionChild(ServerSession*);
};

#endif // SERVER_SESSION_CHILD_HPP
