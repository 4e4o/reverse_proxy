#include "Application.hpp"
#include "Base/ThreadPool.hpp"
#include "Server/ServerInstance.hpp"
#include "Client/ClientInstance.hpp"
#include "Service/ServiceInstance.hpp"
#include "Config.hpp"

#define PROG_NAME   "reverse proxy"
#define DAEMON      false

Application::Application(int argc, char** argv)
    : AApplication(PROG_NAME, argc, argv, DAEMON),
      m_threadPool(new ThreadPool()) {
}

Application::~Application() {
    m_instance.reset();
}

const Config& Application::config() const {
    return *m_config;
}

bool Application::processArgs() {
    po::options_description desc("Options");
    desc.add_options()("c", po::value<std::string>(), "Config file");
    po::variables_map vm = parseCmdLine(desc);

    if (vm.count("c")) {
        m_config.reset(Config::load(vm["c"].as<std::string>()));
    } else {
        log("No config file");
        return false;
    }

    if (m_config.get() == nullptr) {
        log("Config load error");
        return false;
    }

    return true;
}

int Application::exec() {
    if (!processArgs())
        return 1;

    AApplication::exec();

    if (m_config->mode == Mode::CLIENT) {
        m_instance.reset(new ClientInstance(m_threadPool->getIOService()));
    } else if (m_config->mode == Mode::PROXY_SERVER) {
        m_instance.reset(new ServerInstance(m_threadPool->getIOService()));
    } else if (m_config->mode == Mode::SERVICE) {
        m_instance.reset(new ServiceInstance(m_threadPool->getIOService()));
    }

    m_instance->start();

    log("exec start");
    m_threadPool->run();
    m_threadPool->stop(true);
    log("exec end");
    return 0;
}

void Application::onExitRequest() {
    boost::asio::post(m_threadPool->getIOService(), [this]() {
        log("onExitRequest start");
        m_instance->stop();
        m_threadPool->stop(false);
        log("onExitRequest end");
    });
}
