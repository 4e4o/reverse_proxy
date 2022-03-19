#include "Instance.hpp"

Instance::Instance(boost::asio::io_context &io)
    : StrandHolder(io) {
}

Instance::~Instance() {
}
