#ifndef SERVER_SESSION_HPP
#define SERVER_SESSION_HPP

#include "Base/Network/Proxy/ProxyDataSession.hpp"
#include "Protocol/ConnectionType.hpp"

class ServerSession : public ProxyDataSession {
public:
    typedef std::shared_ptr<ServerSession> TSession;

    using ProxyDataSession::ProxyDataSession;
    ~ServerSession();

    void start() override final;

    boost::signals2::signal<void(TSession)> sessionTypeDefined;

    ConnectionType type() const;
    uint8_t serverId() const;

    void requestDataSession();
    void setDataSession(TSession);

private:
    ConnectionType m_type;
    uint8_t m_serverId;
    int m_dataRequestsCount;
    std::array<uint8_t, 2> m_requestBuffer;
};

#endif // SERVER_SESSION_HPP
