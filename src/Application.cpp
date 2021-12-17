#include "Application.hpp"
#include "Base/ThreadPool.hpp"
#include "Server/ServerInstance.hpp"
#include "Client/ClientInstance.hpp"
#include "Service/ServiceInstance.hpp"

#define PROG_NAME   "reverse proxy"
#define DAEMON      false

Application::Application(int argc, char** argv)
    : AApplication(PROG_NAME, argc, argv, DAEMON),
      m_threadPool(new ThreadPool()),
      m_exitStrand(m_threadPool->getIOService()) {
}

Application::~Application() {
    m_instance.reset();
}

bool Application::processArgs() {
    po::options_description desc("Options");

    {
        desc.add_options()("c", "Client mode");
        desc.add_options()("p", "Proxy server mode");
        desc.add_options()("s", "Service mode");

        desc.add_options()("ip", po::value<std::string>(), "Listen ip");
        desc.add_options()("port", po::value<int>(), "Listen port");

        desc.add_options()("ep_ip", po::value<std::string>(), "Endpoint listen ip");
        desc.add_options()("ep_port", po::value<int>(), "Endpoint listen port");

        desc.add_options()("server_id", po::value<int>(), "Server id");
    }

    po::variables_map vm = parseCmdLine(desc);

    if (vm.count("c")) {
        m_mode = Mode::CLIENT;
    } else if (vm.count("p")) {
        m_mode = Mode::PROXY_SERVER;
    } else if (vm.count("s")) {
        m_mode = Mode::SERVICE;
    } else {
        log("Unknown mode");
        return false;
    }

    m_ip = vm["ip"].as<std::string>();
    m_port = vm["port"].as<int>();

    if (vm.count("ep_ip"))
        m_epIp = vm["ep_ip"].as<std::string>();

    if (vm.count("ep_port"))
        m_epPort = vm["ep_port"].as<int>();

    if (vm.count("server_id"))
        m_serverId = vm["server_id"].as<int>();

    return true;
}

int Application::exec() {
    if (!processArgs())
        return 1;

    AApplication::exec();

    if (m_mode == Mode::CLIENT) {
        m_instance.reset(new ClientInstance(m_threadPool->getIOService()));
    } else if (m_mode == Mode::PROXY_SERVER) {
        m_instance.reset(new ServerInstance(m_threadPool->getIOService()));
    } else if (m_mode == Mode::SERVICE) {
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
    boost::asio::post(m_threadPool->getIOService(), m_exitStrand.wrap([this]() {
        m_instance->stop();
        m_threadPool->stop(false);
        log("onExitRequest");
    }));
}

uint8_t Application::serverId() const {
    return m_serverId;
}

int Application::epPort() const {
    return m_epPort;
}

std::string Application::epIp() const {
    return m_epIp;
}

int Application::port() const {
    return m_port;
}

std::string Application::ip() const {
    return m_ip;
}
