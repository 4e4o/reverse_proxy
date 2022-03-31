#ifndef BASE_CLIENT_INSTANCE_HPP
#define BASE_CLIENT_INSTANCE_HPP

#include "Instance.hpp"
#include "Protocol/ConnectionType.hpp"

#include <Network/Session/ISessionProvider.hpp>

class BaseClientInstance : public Instance, public ISessionProvider {
public:
    BaseClientInstance(boost::asio::io_context&, ConnectionType);

private:
    TAwaitSession get(ISessionRequester*) override;

    const ConnectionType m_type;
};

#endif // BASE_CLIENT_INSTANCE_HPP
