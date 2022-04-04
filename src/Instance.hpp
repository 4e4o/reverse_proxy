#ifndef INSTANCE_H
#define INSTANCE_H

#include "Config/ConfigHolder.hpp"
#include "Coroutine/CoroutineTask.hpp"

class Instance : public CoroutineTask<void>, public ConfigHolder {
public:    
    using CoroutineTask::CoroutineTask;
};

#endif // INSTANCE_H
