#ifndef HEADER_APPLICATION_HPP
#define HEADER_APPLICATION_HPP

#include "Base/AApplication.h"

#include <memory>

#define APP         static_cast<Application*>(AAP)
#define CONFIG      APP->config()

class IRunnable;
class ThreadPool;
struct Config;

class Application : public AApplication {
public:
    Application(int argc, char** argv);
    ~Application();

    int exec() override final;

    const Config& config() const;

private:
    void onExitRequest() override;
    bool processArgs();

    std::unique_ptr<Config> m_config;
    std::shared_ptr<ThreadPool> m_threadPool;
    std::shared_ptr<IRunnable> m_instance;
};

#endif // HEADER_APPLICATION_HPP
