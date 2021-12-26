#include "AApplication.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdarg.h>
#include <atomic>

//#define LOG_TO_SYSLOG   1
//#define LOG_TO_CONSOLE  1

AApplication* AApplication::m_app = nullptr;

class AApPrivateAccessor {
public:
    static void callOnExit() {
        AAP->onExitRequest();
    }
};

static std::atomic_flag g_ctrlLock = ATOMIC_FLAG_INIT;

static void ctrCHandler(int) {
    if (g_ctrlLock.test_and_set())
        return;

    AAP->quit();
}

AApplication::AApplication(const std::string& name, int argc, char** argv, bool daemon)
    : m_progName(name), m_argc(argc), m_argv(argv), m_daemonize(daemon) {
    m_app = this;
}

AApplication::~AApplication() {
}

int AApplication::exec() {
    setCtrlCHandler();

    if (m_daemonize)
        daemonize();

    return 0;
}

void AApplication::quit() {
    AApPrivateAccessor::callOnExit();
}

po::variables_map AApplication::parseCmdLine(po::options_description& desc) {
    try {
        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(m_argc, m_argv).options(desc).run();
        po::store(parsed, vm);
        po::notify(vm);
        return vm;
    } catch(std::exception& ex) {
        log("Error parsing command line arguments");
    }

    return po::variables_map();
}

void AApplication::log(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);

#ifdef LOG_TO_CONSOLE
    vprintf(fmt, va);
    printf("\n");
    fflush(stdout);
#endif // LOG_TO_CONSOLE

#ifdef LOG_TO_SYSLOG
    vsyslog(LOG_NOTICE, fmt, va);
#endif // LOG_TO_SYSLOG

    va_end(va);
}

void AApplication::daemonize() {
    /* Fork off the parent process */
    pid_t pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    for (int x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
        close (x);
    }

#ifdef LOG_TO_SYSLOG
    /* Open the log file */
    openlog(m_progName.c_str(), LOG_PID, LOG_DAEMON);
#endif
}

void AApplication::setDaemon(bool d) {
    m_daemonize = d;
}

void AApplication::setCtrlCHandler() {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = ctrCHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}
