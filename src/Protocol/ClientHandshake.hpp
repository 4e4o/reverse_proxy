#ifndef CLIENT_HANDSHAKE_HPP
#define CLIENT_HANDSHAKE_HPP

#include "Base/EnableSharedFromThisVirtual.hpp"

class Session;

class ClientHandshake : public enable_shared_from_this_virtual<ClientHandshake> {
public:
    typedef std::shared_ptr<Session> TSession;
    typedef std::shared_ptr<ClientHandshake> TClientHandshake;

    void startHandshake(TSession, uint8_t, uint8_t, bool stripSSL);

protected:
    virtual void onHandshakeDone(TSession) = 0;

private:
    void sendSessionType(TSession s);
    void onSessionTypeSended(TSession s);
    void finalStep(TSession s);

    bool m_stripSSL;
    uint8_t m_sessionType;
    uint8_t m_serverId;
    std::array<uint8_t, 2> m_sendBuf;
    uint8_t m_success;
};

#endif // CLIENT_HANDSHAKE_HPP
