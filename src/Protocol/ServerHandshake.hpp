#ifndef SERVER_HANDSHAKE_HPP
#define SERVER_HANDSHAKE_HPP

#include "Config/ConfigHolder.hpp"
#include "Protocol/ConnectionType.hpp"

#include <Network/Session/Session.hpp>

// Хэндшйэк для прокси сервера

class ServerHandshake : public Session, public ConfigHolder {
public:
    ServerHandshake(Socket*);
    ServerHandshake(Socket*, ServerHandshake*);
    ~ServerHandshake();

    uint8_t serverId() const;

protected:
    TAwaitConnectionType defineType();
    TAwaitVoid finishHandshake();

private:
    uint8_t m_serverId;
};

#endif // SERVER_HANDSHAKE_HPP
