#ifndef ISERVER_H
#define ISERVER_H

#include <functional>
#include <string>

class Session;

class IServer {
public:
    virtual ~IServer() { }

    virtual void start(const std::string& ip, unsigned short port) = 0;
    virtual void stop() = 0;

    typedef std::function<void(Session*)> TSessionInitEvent;

    void setSessionInit(TSessionInitEvent&& e) {
        m_sessionInit = std::move(e);
    }

protected:

    TSessionInitEvent sessionInit() const {
        return m_sessionInit;
    }

private:
    TSessionInitEvent m_sessionInit;
};

#endif // ISERVER_H
