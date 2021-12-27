#include "Config.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define GET_PARAM(KEY, VAR, TYPE, CRITICAL) { \
    const auto &p = pt.get_optional<TYPE>(KEY); \
    if (p.has_value()) { \
        result->VAR = p.get(); \
    } else { \
        if (CRITICAL) \
            return nullptr; \
    } \
}

std::istream& operator>> (std::istream &in, Mode& m) {
    std::string token;
    in >> token;

    m = Mode::UNKNOWN;

    if (token == "proxy")
        m = Mode::PROXY_SERVER;
    else if (token == "client")
        m = Mode::CLIENT;
    else if (token == "service")
        m = Mode::SERVICE;

    return in;
}

Config* Config::load(const std::string& f) {
    std::unique_ptr<Config> result(new Config());

    result->cryptData = false;

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(f, pt);

    GET_PARAM("Global.Mode", mode, Mode, true);

    if (result->mode == Mode::UNKNOWN)
        return nullptr;

    GET_PARAM("Global.ListenIP", listenIP, std::string, true);
    GET_PARAM("Global.ListenPort", listenPort, int, true);

    if (result->mode == Mode::CLIENT || result->mode == Mode::SERVICE) {
        GET_PARAM("Global.RemoteIP", remoteIP, std::string, true);
        GET_PARAM("Global.RemotePort", remotePort, int, true);
        GET_PARAM("Global.ServerId", serverId, int, true);
    }

    GET_PARAM("Global.VerifyHost", verifyHost, std::string, false);
    GET_PARAM("Global.KeysPath", keysPath, std::string, false);
    GET_PARAM("Global.CryptData", cryptData, bool, false);

    return result.release();
}
