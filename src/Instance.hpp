#ifndef INSTANCE_H
#define INSTANCE_H

#include "Base/IRunnable.hpp"

#include <boost/asio.hpp>

class Instance : public IRunnable {
public:    
    Instance(boost::asio::io_service &io_service)
        : m_ioService(io_service), m_strand(io_service) {
    }

    virtual ~Instance() { }

    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    boost::asio::io_service &io() {
        return m_ioService;
    }

    template <class Callable>
    void post(Callable&& c) {
        boost::asio::post(m_ioService, m_strand.wrap(std::move(c)));
    }

private:
    boost::asio::io_service &m_ioService;
    boost::asio::io_service::strand m_strand;
};

#endif // INSTANCE_H
