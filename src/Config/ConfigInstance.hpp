#ifndef CONFIG_INSTANCE_HPP
#define CONFIG_INSTANCE_HPP

#include <Config/ConfigItem.hpp>

class ConfigInstance : public ConfigItem {
public:
    ConfigInstance(const boost::json::object&);
    ~ConfigInstance();

    enum class Mode {
        SERVICE,
        CLIENT,
        PROXY_SERVER
    };

    Mode mode() const;
    int serverId() const;
    std::string listenIP() const;
    int listenPort() const;
    std::string remoteIP() const;
    int remotePort() const;
    std::string verifyHost() const;
    std::string keysPath() const;
    bool cryptData() const;

private:
    bool init(const boost::json::object&) override final;

    Mode m_mode;
    int m_serverId;
    std::string m_listenIP;
    int m_listenPort;
    std::string m_remoteIP;
    int m_remotePort;
    std::string m_verifyHost;
    std::string m_keysPath;
    bool m_cryptData;
};

#endif // CONFIG_INSTANCE_HPP
