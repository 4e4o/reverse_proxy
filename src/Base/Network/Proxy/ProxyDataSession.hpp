#ifndef PROXY_DATA_SESSION_H
#define PROXY_DATA_SESSION_H

#include "Base/Network/Session.hpp"

// просто прокси сессия, которая передаёт данные другой поксии сессии и получает из неё же.

class ProxyDataSession : public Session {
public:
    using Session::Session;
    ~ProxyDataSession();

    void setOther(std::shared_ptr<ProxyDataSession>);

protected:
    virtual void startProxying(std::shared_ptr<ProxyDataSession> session, bool startSession = true);

private:
    void onOurClose();
    void onOtherSessionWriteDone();
    void onOtherSessionClosed();
    void writeToOther(const uint8_t *ptr, std::size_t size);
    void startImpl() override;

    std::shared_ptr<ProxyDataSession> m_other;
    boost::signals2::connection m_otherOnWrite;
    boost::signals2::connection m_otherOnClose;
};

#endif // PROXY_DATA_SESSION_H
