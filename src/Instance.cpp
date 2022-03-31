#include "Instance.hpp"

Instance::Instance(boost::asio::io_context &io)
    : CoroutineTask(io) {
}

Instance::~Instance() {
}
