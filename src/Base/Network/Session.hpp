#ifndef SESSION_H
#define SESSION_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>
#include <boost/array.hpp>

#include "Base/Span.hpp"

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket&& sock);
    virtual ~Session();

    virtual void start();
    void close();

    virtual void readSome(std::size_t maxSize = READ_BUFFER_SIZE);
    virtual void readAll(std::size_t size);
    virtual void writeAll(const uint8_t *ptr, std::size_t size);

    boost::asio::ip::tcp::socket* socket();

    boost::signals2::signal<void (const uint8_t *ptr, std::size_t size)> onData;
    boost::signals2::signal<void (const boost::system::error_code& ec)> onError;
    boost::signals2::signal<void ()> onWriteDone;
    boost::signals2::signal<void ()> onClose;

    boost::signals2::signal<void ()> onDestroy;

protected:
    boost::asio::io_service &ioService();

    void closeOnWrite();

    template <class Callable>
    void post(Callable&& c) {
        boost::asio::post(m_ioService, m_strand.wrap(std::move(c)));
    }

private:
    static constexpr int READ_BUFFER_SIZE = 64 * 2 * 1024;

    void errorHandler(const boost::system::error_code&);
    bool isDisconnect(const boost::system::error_code&);
    void doWrite();
    void disconnectAllSlots();

    boost::asio::io_service &m_ioService;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::io_service::strand m_strand;
    std::array<uint8_t, READ_BUFFER_SIZE> m_readBuffer;
    span<const uint8_t> m_writeSpan;
    bool m_writing;
    bool m_closeOnWrite;
    bool m_closed;
};

#endif // SESSION_H
