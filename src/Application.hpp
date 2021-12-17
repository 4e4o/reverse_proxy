#ifndef HEADER_APPLICATION_HPP
#define HEADER_APPLICATION_HPP

#include "Base/AApplication.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

#include <optional>
#include <list>

#define APP         static_cast<Application*>(AAP)

class IRunnable;
class ThreadPool;

class Application : public AApplication {
public:
    Application(int argc, char** argv);
    ~Application();

    int exec() override final;

    enum class Mode {
        SERVICE,
        CLIENT,
        PROXY_SERVER
    };

    std::string ip() const;
    int port() const;

    std::string epIp() const;
    int epPort() const;

    uint8_t serverId() const;

private:
    void onExitRequest() override;
    bool processArgs();

    std::string m_ip;
    int m_port;
    std::string m_epIp;
    int m_epPort;
    uint8_t m_serverId;

    std::shared_ptr<ThreadPool> m_threadPool;
    std::shared_ptr<IRunnable> m_instance;
    boost::asio::io_service::strand m_exitStrand;
    Mode m_mode;
};

#endif // HEADER_APPLICATION_HPP
