#ifndef CLIENT_HANDSHAKE_HPP
#define CLIENT_HANDSHAKE_HPP

#include "Config/ConfigHolder.hpp"
#include "Protocol/ConnectionType.hpp"

#include <Network/Session/Session.hpp>

// Хэндшйэк для клиентов по отношению к прокси серверу

class ClientHandshake : public Session, public ConfigHolder {
public:
    ClientHandshake(Socket*);
    ~ClientHandshake();

    void setSkipSSLStrip(bool skipSSLStrip);
    void setSessionType(const ConnectionType &sessionType);

protected:
    TAwaitVoid prepare() override;
    TAwaitVoid work() override;

private:
    bool m_skipSSLStrip;
    ConnectionType m_sessionType;
};

using ClientSession = ClientHandshake;

#endif // CLIENT_HANDSHAKE_HPP
