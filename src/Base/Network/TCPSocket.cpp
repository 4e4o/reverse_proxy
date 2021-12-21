#include "TCPSocket.hpp"
#include "Base/AApplication.h"

static boost::asio::ssl::context initContext() {
    using namespace boost::asio::ssl;

    context ctx(boost::asio::ssl::context::tls);
    ctx.set_options(context::default_workarounds
                             | context::no_sslv2
                             | context::no_sslv3
                             | context::no_tlsv1
                             | context::no_tlsv1_1);

    ctx.load_verify_file("ca.crt");
    ctx.use_certificate_file("entity.crt", context::pem);
    ctx.use_private_key_file("entity.key", context::pem);
    return ctx;
}

TCPSocket::TCPSocket(boost::asio::io_service &io)
    : m_ssl(false),
      m_sslContext(initContext()),
      m_socket(io, m_sslContext) {
}

TCPSocket::TCPSocket(TCPSocket&& socket)
    : TCPSocket(std::move(socket.m_socket.next_layer())) {
}

TCPSocket::TCPSocket(boost::asio::ip::tcp::socket&& socket)
    : m_ssl(false),
      m_sslContext(initContext()),
      m_socket(std::move(socket), m_sslContext) {
}

TCPSocket::~TCPSocket() {
}

void TCPSocket::close() {
    m_socket.next_layer().close();
}

void TCPSocket::initSSL(boost::asio::ssl::stream_base::handshake_type type,
                        const std::string& verifyHost, TInitResult r) {
    using namespace boost::asio::ssl;

    m_ssl = true;
    m_socket.set_verify_mode(verify_peer);

#if BOOST_VERSION < 107300
    m_socket.set_verify_callback(rfc2818_verification(verifyHost));
#else
    m_socket.set_verify_callback(host_name_verification(verifyHost));
#endif // BOOST_VERSION < 107300

    m_socket.async_handshake(type,
                             [r] (boost::system::error_code error) {
        AAP->log("AAAA %s", error.message().c_str());
        r(!error);
    });
}

void TCPSocket::setSSL(bool enabled) {
    m_ssl = enabled;
}