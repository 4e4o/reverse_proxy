#include "Application.hpp"
#include "Server/ServerInstance.hpp"
#include "Client/ClientInstance.hpp"
#include "Service/ServiceInstance.hpp"
#include "Config/ConfigInstance.hpp"

#include <Config/Config.hpp>

Application::Application(int argc, char** argv)
    : BaseConfigApplication(argc, argv) {
    config()->registerType<ConfigItem, ConfigInstance, const boost::json::object&>();
}

Application::~Application() {
    m_instances.clear();
}

bool Application::start(TConfigItems& items) {
    for (auto& i : items) {
        ConfigInstance *ci = static_cast<ConfigInstance*>(i.get());

        if (ci->mode() == ConfigInstance::Mode::CLIENT) {
            m_instances.emplace_back(new ClientInstance(io()));
        } else if (ci->mode() == ConfigInstance::Mode::PROXY_SERVER) {
            m_instances.emplace_back(new ServerInstance(io()));
        } else if (ci->mode() == ConfigInstance::Mode::SERVICE) {
            m_instances.emplace_back(new ServiceInstance(io()));
        }

        m_instances.back()->setConfig(ci);
        m_instances.back()->start();
    }

    return true;
}

void Application::doExit() {
    for (auto& i : m_instances)
        i->stop();
}
