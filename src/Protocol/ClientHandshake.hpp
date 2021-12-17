#ifndef CLIENT_HANDSHAKE_HPP
#define CLIENT_HANDSHAKE_HPP

#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>

class Session;

class ClientHandshake : public std::enable_shared_from_this<ClientHandshake> {
public:
    typedef std::shared_ptr<Session> TSession;

    ClientHandshake(uint8_t type, uint8_t id);

    void start(TSession);

    boost::signals2::signal<void()> onSuccessResponse;

private:
    void sendSessionType(TSession s);
    void onSessionTypeSended(TSession s);

    uint8_t m_sessionType;
    uint8_t m_serverId;
};

#endif // CLIENT_HANDSHAKE_HPP
