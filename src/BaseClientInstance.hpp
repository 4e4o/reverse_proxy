#ifndef BASE_CLIENT_INSTANCE_HPP
#define BASE_CLIENT_INSTANCE_HPP

#include "Instance.hpp"
#include "Protocol/ConnectionType.hpp"

#include <boost/signals2/signal.hpp>

class Session;

class BaseClientInstance : public Instance {
public:
    BaseClientInstance(boost::asio::io_context &io);

protected:
    void proxy(ConnectionType, std::shared_ptr<Session>);

    boost::signals2::signal<void ()> stopped;
};

#endif // BASE_CLIENT_INSTANCE_HPP
