#ifndef CONNECTION_TYPE_HPP
#define CONNECTION_TYPE_HPP

#include <boost/asio/awaitable.hpp>

#include <stdint.h>

enum class ConnectionType : uint8_t {
    CLIENT,                   // клиент коннектится к прокси серверу
    SERVICE_CLIENT_CONTROL,   // клиент от сервиса коннектится к прокси серверу, контрольное соединение
    SERVICE_CLIENT_DATA       // клиент сервиса коннектится к прокси серверу, проксируемые данные
};

typedef boost::asio::awaitable<ConnectionType> TAwaitConnectionType;

#endif // CONNECTION_TYPE_HPP
