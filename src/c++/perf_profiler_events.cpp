#include "perf_profiler_events.hpp"
#include "perf_profiler_exception.hpp"

#include <iostream>

#include <sys/wait.h>


PerfProfilerEvents::PerfProfilerEvents()
{
    ::memset(&this->pe_, 0, sizeof(struct perf_event_attr));

    this->pe_.type           = PERF_TYPE_HARDWARE;
    this->pe_.size           = sizeof(struct perf_event_attr);
    this->pe_.config         = PERF_COUNT_HW_INSTRUCTIONS;
    this->pe_.disabled       = 1;
    this->pe_.exclude_kernel = 1;
    this->pe_.exclude_hv     = 1;
}

void PerfProfilerEvents::run(int argc, char **argv)
{
    pid_t childPid = 0;

    switch (childPid = ::fork())
    {
        case -1:
            throw PerfProfilerException("Can't create child, fork() failed");
            break;

        case 0:
            PerfProfilerEvents::executeChild_(argc, argv);
            break;

        default: 
            PerfProfilerEvents::executeParent_(childPid);
            break;
    }
}

void PerfProfilerEvents::executeChild_(int argc, char **argv)
{
    // TODO: add support for testing programm args
    char *args[] = { argv[1], NULL };
    ::execve(argv[1], args, NULL);

    // Never reach
    ::_exit(EXIT_SUCCESS);
}

void PerfProfilerEvents::executeParent_(const pid_t childPid)
{
    auto status = 0;
    auto fd     = perf_event::open(this->pe_, childPid, -1, -1, 0);

    perf_event::start(fd, true);

    while (true)
    {
        auto waitPid = ::waitpid(childPid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (waitPid == -1)
            throw PerfProfilerException("waitpid(), error");

        if (waitPid == childPid)
        {
            // Child ended
            if (WIFEXITED(status))
            {
                std::cout << "Child " << childPid << " ended normally" << std::endl;
            }
            else if (WIFSIGNALED(status))
            {
                std::cerr << "Child " << childPid << " ended because of an uncaught signal" << std::endl;
            }
            else if (WIFSTOPPED(status))
            {
                std::cerr << "Child " << childPid << " has stopped" << std::endl;
            }

            break; // Root exit point
        }
    }

    perf_event::stop(fd, true);

    {
        // TODO: move to view module
        long long count = 0;
        ::read(fd, &count, sizeof(long long));
        std::cout << "Used " << count << " instructions" << std::endl;
    }

    perf_event::close(fd);
}
