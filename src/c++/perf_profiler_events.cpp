#include "perf_profiler_events.hpp"

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
            throw std::string("fork() failed"); // TODO: class Exception

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
    char *args[] = { argv[1], NULL };
    ::execve(argv[1], args, NULL);

    // Never reach
    ::_exit(EXIT_SUCCESS);
}

void PerfProfilerEvents::executeParent_(const pid_t childPid)
{
    int status = 0;
    auto fd    = PerfEvent::open(this->pe_, childPid, -1, -1, 0);

    PerfEvent::start(fd, true);

    while (true)
    {
        auto waitPid = ::waitpid(childPid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (waitPid == -1)
            throw(std::string("waitpid(), error")); // TODO: class Exception

        if (waitPid == 0)
        {
            // Child still running
        }
        else if (waitPid == childPid)
        {
            // Child ended
            if (WIFEXITED(status))
            {
                std::cout << "Child " << childPid << "  ended normally" << std::endl;
            }
            else if (WIFSIGNALED(status))
            {
                std::cerr << "Child " << childPid << " ended because of an uncaught signal" << std::endl;
            }
            else if (WIFSTOPPED(status))
            {
                std::cerr << "Child " << childPid << " %d has stopped" << std::endl;
            }

            break;
        }
    }

    PerfEvent::stop(fd, true);

    {
        long long count;
        ::read(fd, &count, sizeof(long long));
        printf("Used %lld instructions\n", count);
    }

    PerfEvent::close(fd);
}
