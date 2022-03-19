#ifndef SERVER_HANDSHAKE_HPP
#define SERVER_HANDSHAKE_HPP

#include "Config/ConfigHolder.hpp"
#include "Protocol/ConnectionType.hpp"

#include <memory>

class Session;

// Хэндшйэк для прокси сервера

class ServerHandshake : public ConfigHolder {
public:
    ServerHandshake(Session*);
    ServerHandshake(Session*, ServerHandshake*);
    ~ServerHandshake();

    void startHandshake();

    ConnectionType type() const;
    uint8_t serverId() const;

protected:
    virtual void onClient() { }
    virtual void onClientData() { }
    virtual void onHandshakeDone() { }

    void finishHandshake();

private:
    void readClientType();
    void onControl();

    ConnectionType m_type;
    uint8_t m_serverId;
    Session *m_session;
};

#endif // SERVER_HANDSHAKE_HPP
