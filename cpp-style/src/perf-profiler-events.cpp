#include <iostream>

#include <sys/wait.h>

#include <perf-utils.hpp>
#include <perf-exception.hpp>
#include <perf-profiler-events.hpp>


PerfProfilerEvents::PerfProfilerEvents()
{
    std::cout << __FUNCTION__ << " Invoke" << std::endl;

    ::memset(&this->pe_, 0, sizeof(struct perf_event_attr));

    this->pe_.type           = PERF_TYPE_HARDWARE;
    this->pe_.size           = sizeof(struct perf_event_attr);
    this->pe_.config         = PERF_COUNT_HW_INSTRUCTIONS;
    this->pe_.disabled       = 1;
    this->pe_.exclude_kernel = 1;
    this->pe_.exclude_hv     = 1;

    std::cout << __FUNCTION__ << " Done" << std::endl;
}

PerfProfilerEvents::PerfProfilerEvents(std::vector<uint32_t>& hw) : hw_(hw)
{
    ::memset(&this->pe_, 0, sizeof(struct perf_event_attr));

    this->pe_.type           = PERF_TYPE_HARDWARE;
    this->pe_.size           = sizeof(struct perf_event_attr);
    this->pe_.config         = this->hw_[0];
    this->pe_.disabled       = 1;
    this->pe_.exclude_kernel = 1;
    this->pe_.exclude_hv     = 1;
    this->pe_.read_format    = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    this->hw_id_  = std::vector<uint64_t>(this->hw_.size());
    this->hw_val_ = std::vector<uint64_t>(this->hw_.size());
}

void PerfProfilerEvents::run(int argc, char **argv)
{
    pid_t childPid = 0;

    try
    {
        switch (childPid = ::fork())
        {
            case -1:
                throw Exception("Can't create child, fork() failed");
                break;

            case 0:
                PerfProfilerEvents::executeChild_(argc, argv);
                break;

            default: 
                PerfProfilerEvents::executeParent_(childPid);
                break;
        }
    }
    catch (Exception& exception)
    {
        std::cerr << exception.what() << " : " << exception.code() << std::endl;
    }
    catch (std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unexpected exception" << std::endl;
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
    auto perfEvent = PerfEvent(this->pe_, childPid, this->hw_, this->hw_id_);

    perfEvent.start();

    while (true)
    {
        auto waitPid = ::waitpid(childPid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (waitPid == -1)
            throw Exception("waitpid(), error");

        if (waitPid == childPid)
        {
            // Child ended
            if (WIFEXITED(status))
            {
                std::cout << "Child " << childPid << " ended normally" << std::endl;
            }
            else if (WIFSIGNALED(status))
            {
                throw Exception("Child " + std::to_string(childPid) + " ended because of an uncaught signal");
            }
            else if (WIFSTOPPED(status))
            {
                throw Exception("Child " + std::to_string(childPid) + " has stopped");
            }

            break; // Root exit point
        }
    }

    perfEvent.stop();

    {
        // TODO: move to view module
        char buf[4096] = { 0 };
        read_format_t *rf = (read_format_t *)buf;

        ::read(perfEvent.getFd(), buf, sizeof(buf));

        for (size_t i = 0; i < rf->nr; i++)
        {
            for (size_t j = 0; j < rf->nr; j++)
            {
               if (rf->values[i].id == this->hw_id_[j])
                {
                    this->hw_val_[j] = rf->values[i].value;
                    break;
                }
            }
        }

        std::cout << "PERF_COUNT_HW_CPU_CYCLES   : " << (this->hw_val_[0] ? this->hw_val_[0] : -1) << std::endl;
        std::cout << "PERF_COUNT_HW_INSTRUCTIONS : " << (this->hw_val_[1] ? this->hw_val_[1] : -1) << std::endl;
    }
}
