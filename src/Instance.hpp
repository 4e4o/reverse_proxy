#ifndef INSTANCE_H
#define INSTANCE_H

#include "Config/ConfigHolder.hpp"
#include "Coroutine/CoroutineTask.hpp"

class ConfigInstance;

class Instance : public CoroutineTask<void>, public ConfigHolder {
public:    
    Instance(boost::asio::io_context &io);
    virtual ~Instance();
};

#endif // INSTANCE_H
