#include "perf_profiler_sampling.hpp"

#include <iostream>

#include <sys/wait.h>


PerfProfilerSampling::PerfProfilerSampling(uint32_t samplePeriod)
{
    memset(&(this->pe_), 0, sizeof(struct perf_event_attr));

    this->pe_.type           = PERF_TYPE_SOFTWARE;
    this->pe_.config         = PERF_COUNT_SW_TASK_CLOCK;
    this->pe_.size           = sizeof(struct perf_event_attr);
    this->pe_.disabled       = 1;
    this->pe_.exclude_kernel = 1;
    this->pe_.exclude_hv     = 1;
    this->pe_.read_format    = PERF_FORMAT_ID;
    this->pe_.sample_period  = samplePeriod; // Time between samples
    this->pe_.sample_id_all  = 1;
    this->pe_.sample_type    = PERF_SAMPLE_IP | PERF_SAMPLE_TID;
}

void PerfProfilerSampling::run(int argc, char **argv)
{
    pid_t childPid = 0;

    switch (childPid = ::fork())
    {
        case -1:
            throw std::string("fork() failed"); // TODO: class Exception

        case 0:
            executeChild_(argc, argv);
            break;

        default: 
            executeParent_(childPid);
            break;
    }
}

void PerfProfilerSampling::executeChild_(int argc, char **argv)
{
    char *args[] = { argv[1], NULL };
    ::execve(argv[1], args, NULL);

    // Never reach
    ::_exit(EXIT_SUCCESS);
}

void PerfProfilerSampling::executeParent_(const pid_t childPid)
{
    int status = 0;

    auto samplesCnt = 0;
    auto fd         = PerfEvent::open(this->pe_, childPid, -1, -1, 0);
    auto ringBuffer = PerfEvent::RingBuffer(fd);

    PerfEvent::start(fd, true);

    while (true)
    {
        pid_t waitPid = ::waitpid(childPid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (waitPid == -1)
            throw(std::string("waitpid(), error")); // TODO: class Exception

        if (waitPid == 0) // Child still running 
        {
            if (ringBuffer.hasData())
            {
                auto page   = ringBuffer.pageGet();
                auto sample = ringBuffer.sampleGet();

                PerfProfilerSampling::pagePrint_(page);
                PerfProfilerSampling::samplePrint_(sample);

                samplesCnt++;
            }
        }
        else if (waitPid == childPid) // Child ended
        {
            if (WIFEXITED(status))
            {
                std::cout << "Samples gather: " << samplesCnt << std::endl;
                std::cout << "Child " << childPid << " ended normally" << std::endl;
            }
            else if (WIFSIGNALED(status))
            {
                std::cerr << "Child " << childPid << " ended because of an uncaught signal" << std::endl;
            }
            else if (WIFSTOPPED(status))
            {
                std::cerr << "Child " << childPid << " %d has stopped" << std::endl;
            }

            break; // Exit point
        }
    }

    PerfEvent::stop(fd, true);

    PerfEvent::close(fd);
}

void PerfProfilerSampling::pagePrint_(const PerfEvent::RecordPage& page)
{
    std::cout << "The first metadata mmap page:"                    << std::endl;
    std::cout << "\tversion        : " << page.mpage.version        << std::endl;
    std::cout << "\tcompat version : " << page.mpage.compat_version << std::endl;
    std::cout << "\tlock           : " << page.mpage.lock           << std::endl;
    std::cout << "\tindex          : " << page.mpage.index          << std::endl;
    std::cout << "\toffset         : " << page.mpage.offset         << std::endl;
    std::cout << "\ttime enabled   : " << page.mpage.time_enabled   << std::endl;
    std::cout << "\ttime running   : " << page.mpage.time_running   << std::endl;
    std::cout << "\tdata head      : " << page.mpage.data_head      << std::endl;
    std::cout << "\tdata tail      : " << page.mpage.data_tail      << std::endl;
    std::cout << "\tdata offset    : " << page.mpage.data_offset    << std::endl;
    std::cout << "\tdata size      : " << page.mpage.data_size      << std::endl;
    std::cout                                                       << std::endl;
}

void PerfProfilerSampling::samplePrint_(const PerfEvent::RecordSample& sample)
{
    if (sample.header.type != PERF_RECORD_SAMPLE)
        return;

    std::cout << "Sample's header:"                << std::endl;
    std::cout << "\ttype : PERF_RECORD_SAMPLE"     << std::endl;
    std::cout << "\tmisc : " << sample.header.misc << std::endl;
    std::cout << "\tsize : " << sample.header.size << std::endl;
    std::cout                                      << std::endl;
    std::cout << "Sample's data:"                  << std::endl;
    std::cout << "\ttid  : "   << sample.tid       << std::endl;
    std::cout << "\tpid  : "   << sample.pid       << std::endl;
    std::cout << "\tip   : 0x" << sample.ip        << std::endl;
    std::cout                                      << std::endl;
}
