#ifndef HEADER_APPLICATION_HPP
#define HEADER_APPLICATION_HPP

#include <BaseConfigApplication.h>

#include <list>

class Instance;

class Application : public BaseConfigApplication {
public:
    Application(int argc, char** argv);
    ~Application();

    bool start(TConfigItems&) override final;

private:
    void doExit() override final;

    typedef std::shared_ptr<Instance> TInstancePtr;
    std::list<TInstancePtr> m_instances;
};

#endif // HEADER_APPLICATION_HPP
