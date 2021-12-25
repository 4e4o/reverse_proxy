#ifndef SERVER_SESSION_HPP
#define SERVER_SESSION_HPP

#include "Base/Network/Proxy/ProxyDataSession.hpp"
#include "Protocol/ConnectionType.hpp"

class ServerSession : public ProxyDataSession {
public:
    typedef std::shared_ptr<ServerSession> TSession;

    using ProxyDataSession::ProxyDataSession;
    ~ServerSession();

    boost::signals2::signal<void(TSession)> onControlSession;
    boost::signals2::signal<void(TSession)> onClientSession;
    boost::signals2::signal<void(TSession)> onClientDataSession;

    ConnectionType type() const;
    uint8_t serverId() const;

    void requestDataSession();
    void setDataSession(TSession);

private:
    typedef std::function<void()> TEvent;

    void readClientType();
    void startImpl() override final;
    void sendSuccess();
    void onControl();
    void onClient();
    void onClientData();
    void processDataSessionRequest();
    void finishClientHandshake(TEvent);

    uint8_t m_success;
    ConnectionType m_type;
    uint8_t m_serverId;
    int m_dataRequestsCount;
    bool m_requestWriting;
    std::array<uint8_t, 2> m_requestBuffer;
};

#endif // SERVER_SESSION_HPP
