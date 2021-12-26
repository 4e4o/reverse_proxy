#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

enum class Mode {
    UNKNOWN,
    SERVICE,
    CLIENT,
    PROXY_SERVER
};

struct Config {
    Mode mode;
    std::string listenIP;
    int listenPort;
    int serverId;
    std::string remoteIP;
    int remotePort;
    std::string verifyHost;
    std::string keysPath;

    static Config* load(const std::string&);
};

#endif // CONFIG_HPP
