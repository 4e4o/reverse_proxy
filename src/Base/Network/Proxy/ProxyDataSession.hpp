#ifndef PROXY_DATA_SESSION_H
#define PROXY_DATA_SESSION_H

#include "Base/Network/Session.hpp"

// просто прокси сессия, которая передаёт данные другой поксии сессии и получает из неё же.

class ProxyDataSession : public Session {
public:
    ProxyDataSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock);
    ~ProxyDataSession();

    void setOther(std::shared_ptr<ProxyDataSession>);
    virtual void start() override;

private:
    void onOurClose();
    void onOtherSessionWriteDone();
    void onOtherSessionClosed();
    void writeToOther(const uint8_t *ptr, std::size_t size);

    std::shared_ptr<ProxyDataSession> m_other;
    boost::signals2::connection m_otherOnWrite;
    boost::signals2::connection m_otherOnClose;
};

#endif // PROXY_DATA_SESSION_H
