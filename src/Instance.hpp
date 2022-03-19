#ifndef INSTANCE_H
#define INSTANCE_H

#include "Misc/IRunnable.hpp"
#include "Misc/StrandHolder.hpp"
#include "Config/ConfigHolder.hpp"

class ConfigInstance;

class Instance : public StrandHolder, public IRunnable, public ConfigHolder {
public:    
    Instance(boost::asio::io_context &io);
    virtual ~Instance();
};

#endif // INSTANCE_H
