#include "Session.hpp"
#include "Base/AApplication.h"

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

Session::Session(boost::asio::io_service &io)
    : Session(io, TCPSocket(io)) {
}

Session::Session(boost::asio::io_service &io, TCPSocket &&sock)
    : m_io(io),
      m_socket(std::move(sock)),
      m_strand(io),
      m_writing(false),
      m_closeOnWrite(false),
      m_closed(false) {
    AAP->log("Session %p", this);
}

Session::~Session() {
    onDestroy();
    AAP->log("~Session %p", this);
}

void Session::start() {
    m_socket.init();
    startImpl();
}

void Session::startImpl() {
}

void Session::readAll(std::size_t size) {
    auto self = shared_from_this();
    boost::asio::async_read(m_socket, boost::asio::buffer(m_readBuffer, size),
                            m_strand.wrap([self, size](const boost::system::error_code &ec,
                                          std::size_t bytes_transferred) {
        if (self->m_closed)
            return;

        if (ec || (bytes_transferred != size)) {
            self->errorHandler(ec);
            return;
        }

        self->onData(self->m_readBuffer.data(), bytes_transferred);
    }));
}

void Session::readSome(std::size_t maxSize) {
    const std::size_t size = std::min(maxSize, m_readBuffer.size());
    auto self = shared_from_this();
    m_socket.async_read_some(
                boost::asio::buffer(m_readBuffer, size),
                m_strand.wrap([self](const boost::system::error_code &ec,
                              std::size_t bytes_transferred) {
        if (self->m_closed)
            return;

        if (ec) {
            self->errorHandler(ec);
            return;
        }

        // AAP->log("REEEEEAD %i %x %p", bytes_transferred, self->m_readBuffer.data()[0] & 0xFF, self.get());
        self->onData(self->m_readBuffer.data(), bytes_transferred);
    }));
}

void Session::writeAll(const uint8_t *ptr, std::size_t size) {
    auto self = shared_from_this();
    m_writeSpan = {ptr, size};
    post([self]() {
        if (self->m_closed)
            return;

        self->doWrite();
    });
}

void Session::doWrite() {
    if (m_writing) {
        AAP->log("Session::doWrite m_writing == true, FIX IT !!!!! %p", this);
        return;
    }

    m_writing = true;
    auto self = shared_from_this();

    boost::asio::async_write(
                m_socket, boost::asio::buffer(m_writeSpan.data(), m_writeSpan.size()),
                m_strand.wrap([self](boost::system::error_code ec,
                              std::size_t bytes_transferred) {
                    if (self->m_closed) {
                        return;
                    }

                    if (ec || (self->m_writeSpan.size() != bytes_transferred)) {
                        self->errorHandler(ec);
                        return;
                    }

                    self->m_writing = false;
                    self->onWriteDone();

                    if (self->m_closeOnWrite) {
                        self->closeOnWrite();
                    }
                }));
}

TCPSocket& Session::socket() {
    return m_socket;
}

void Session::startSSL(bool client, const std::string& verifyHost, TEvent r) {
    auto self = shared_from_this();

    socket().initSSL(client ? boost::asio::ssl::stream_base::handshake_type::client :
                              boost::asio::ssl::stream_base::handshake_type::server,
                     verifyHost, [self, r](bool success) {
        if (!success) {
            self->close();
            return;
        }

        r();
    });
}

boost::asio::io_service &Session::io() {
    return m_io;
}

void Session::errorHandler(const boost::system::error_code& ec) {
    if (!isDisconnect(ec)) {
        AAP->log("Session::errorHandler %p %s", this, ec.message().c_str());
        onError(ec);
    }

    close();
}

bool Session::isDisconnect(const boost::system::error_code& code) {
    if ((boost::asio::error::eof == code) ||
            (boost::asio::error::connection_reset == code))
        return true;

    return false;
}

void Session::close() {
    auto self = shared_from_this();

    post([self]() {
        if (self->m_closed)
            return;

        try {
            self->m_socket.close();
            self->m_closed = true;
            self->onClose();
            self->disconnectAllSlots();
            //AAP->log("socket close ok %p", self.get());
        } catch (std::exception& e) {
            AAP->log("socket close exception: %p, %s", self.get(), e.what());
        }
    });
}

void Session::disconnectAllSlots() {
    onData.disconnect_all_slots();
    onError.disconnect_all_slots();
    onWriteDone.disconnect_all_slots();
    onClose.disconnect_all_slots();
    // onDestroy не дисконектим, он должен вызываться в деструкторе
}

void Session::closeOnWrite() {
    auto self = shared_from_this();

    post([self]() {
        self->m_closeOnWrite = true;

        //        AAP->log("Session::closeOnWrite %p %i", self.get(), self->m_writing);

        if (!self->m_writing)
            self->close();
    });
}
