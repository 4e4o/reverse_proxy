#ifndef AAPPLICATION_H
#define AAPPLICATION_H

#include <boost/program_options.hpp>

#include <string>

#define AAP AApplication::app()

namespace po = boost::program_options;

class AApplication {
public:
    AApplication(const std::string&, int argc, char** argv, bool daemon = false);
    ~AApplication();

    void log(const char *fmt, ...);

    static AApplication* app() {
        return m_app;
    }

    virtual int exec();
    void quit();

protected:
    void daemonize();
    void setDaemon(bool);

    po::variables_map parseCmdLine(po::options_description&);
    virtual void onExitRequest() { }

private:
    friend class AApPrivateAccessor;

    void setCtrlCHandler();

    std::string m_progName;
    int m_argc;
    char** m_argv;
    bool m_daemonize;

    static AApplication* m_app;
};

#endif // AAPPLICATION_H
