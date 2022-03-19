#include "ConfigHolder.hpp"

ConfigHolder::ConfigHolder()
    : m_config(nullptr) {
}

ConfigHolder::~ConfigHolder() {
}

const ConfigInstance *ConfigHolder::config() const {
    return m_config;
}

void ConfigHolder::setConfig(const ConfigInstance *config) {
    m_config = config;
}
