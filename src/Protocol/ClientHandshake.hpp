#ifndef CLIENT_HANDSHAKE_HPP
#define CLIENT_HANDSHAKE_HPP

#include "Config/ConfigHolder.hpp"
#include "Protocol/ConnectionType.hpp"

#include <memory>

class Session;

// Хэндшйэк для клиентов по отношению к прокси серверу

class ClientHandshake : public ConfigHolder {
public:
    ClientHandshake(Session*);
    ~ClientHandshake();

    void setSkipSSLStrip(bool skipSSLStrip);
    void setSessionType(const ConnectionType &sessionType);

    void startHandshake();

protected:
    virtual void onHandshakeDone() = 0;

private:
    void sendSessionType();
    void onSessionTypeSended();
    void finalStep();

    bool m_skipSSLStrip;
    ConnectionType m_sessionType;
    Session* m_session;
};

#endif // CLIENT_HANDSHAKE_HPP
