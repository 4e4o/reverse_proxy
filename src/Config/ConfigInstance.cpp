#include "ConfigInstance.hpp"

ConfigInstance::ConfigInstance(const boost::json::object&)
    : m_mode(Mode::SERVICE),
      m_serverId(0),
      m_cryptData(false) {
}

ConfigInstance::~ConfigInstance() {
}

bool ConfigInstance::init(const boost::json::object &obj) {
    const std::string mode = get<std::string>(obj, "Mode");

    if (mode == "proxy")
        m_mode = Mode::PROXY_SERVER;
    else if (mode == "client")
        m_mode = Mode::CLIENT;
    else if (mode == "service")
        m_mode = Mode::SERVICE;
    else
        return false;

    m_listenIP = get<std::string>(obj, "ListenIP");
    m_listenPort = get<std::int64_t>(obj, "ListenPort");

    if (m_mode == Mode::CLIENT || m_mode == Mode::SERVICE) {
        m_remoteIP = get<std::string>(obj, "RemoteIP");
        m_remotePort = get<std::int64_t>(obj, "RemotePort");
        m_serverId = get<std::int64_t>(obj, "ServerId");
    }

    m_verifyHost = get<std::string, true>(obj, "VerifyHost");
    m_keysPath = get<std::string, true>(obj, "KeysPath");
    m_cryptData = get<std::int64_t, true>(obj, "CryptData");

    return true;
}

ConfigInstance::Mode ConfigInstance::mode() const {
    return m_mode;
}

int ConfigInstance::serverId() const {
    return m_serverId;
}

std::string ConfigInstance::listenIP() const {
    return m_listenIP;
}

int ConfigInstance::listenPort() const {
    return m_listenPort;
}

std::string ConfigInstance::remoteIP() const {
    return m_remoteIP;
}

int ConfigInstance::remotePort() const {
    return m_remotePort;
}

std::string ConfigInstance::verifyHost() const {
    return m_verifyHost;
}

std::string ConfigInstance::keysPath() const {
    return m_keysPath;
}

bool ConfigInstance::cryptData() const {
    return m_cryptData;
}
