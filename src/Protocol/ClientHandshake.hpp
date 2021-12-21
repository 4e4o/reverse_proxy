#ifndef CLIENT_HANDSHAKE_HPP
#define CLIENT_HANDSHAKE_HPP

#include <memory>

class Session;

class ClientHandshake {
public:
    typedef std::shared_ptr<Session> TSession;

    void startHandshake(TSession, uint8_t, uint8_t);

protected:
    virtual void onHandshakeDone(TSession) = 0;

private:
    void sendSessionType(TSession s);
    void onSessionTypeSended(TSession s);

    uint8_t m_sessionType;
    uint8_t m_serverId;
    std::array<uint8_t, 2> m_sendBuf;
};

#endif // CLIENT_HANDSHAKE_HPP
