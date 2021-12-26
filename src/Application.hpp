#ifndef HEADER_APPLICATION_HPP
#define HEADER_APPLICATION_HPP

#include "Base/AApplication.h"

#include <memory>

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

    bool m_stopped;
    std::shared_ptr<ThreadPool> m_threadPool;
    std::shared_ptr<IRunnable> m_instance;
    Mode m_mode;
};

#endif // HEADER_APPLICATION_HPP
