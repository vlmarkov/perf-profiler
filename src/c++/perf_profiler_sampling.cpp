#include "perf_profiler_sampling.hpp"
#include "perf_profiler_exception.hpp"

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

    this->samplesCnt_ = 0;
}

void PerfProfilerSampling::run(int argc, char **argv)
{
    pid_t childPid = 0;

    switch (childPid = ::fork())
    {
        case -1:
            throw PerfProfilerException("Can't create child, fork() failed");
            break;

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
    // TODO: add support for testing programm args
    char *args[] = { argv[1], NULL };
    ::execve(argv[1], args, NULL);

    // Never reach
    ::_exit(EXIT_SUCCESS);
}

void PerfProfilerSampling::executeParent_(const pid_t childPid)
{
    auto status     = 0;
    auto fd         = perf_event::open(this->pe_, childPid, -1, -1, 0);
    auto ringBuffer = perf_event::RingBuffer(fd);

    perf_event::start(fd, true);

    while (true)
    {
        pid_t waitPid = ::waitpid(childPid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (waitPid == -1)
            throw PerfProfilerException("::waitpid(), error");

        if (waitPid == 0) // Child still running 
        {
            if (ringBuffer.hasData())
            {
                auto page   = ringBuffer.pageGet();
                auto sample = ringBuffer.sampleGet();

                PerfProfilerSampling::pagePrint_(page);
                PerfProfilerSampling::samplePrint_(sample);
            }
        }
        else if (waitPid == childPid) // Child ended
        {
            if (WIFEXITED(status))
            {
                std::cout << "Samples gather: " << this->samplesCnt_ << std::endl;
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

    perf_event::close(fd);

    PerfProfilerSampling::mapPrint_();
}

// TODO: move to view module
void PerfProfilerSampling::pagePrint_(const perf_event::RecordPage& page)
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

// TODO: move to view module
void PerfProfilerSampling::samplePrint_(const perf_event::RecordSample& sample)
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
    std::cout                                      << std::hex;
    std::cout << "\tip   : 0x" << sample.ip        << std::endl;
    std::cout                                      << std::dec;
    std::cout                                      << std::endl;

    // TODO: move to model module
    auto find = this->map_.find(sample.ip);
    if (find != this->map_.end())
        find->second++;
    else
        this->map_.insert(std::make_pair(sample.ip, 1));

    this->samplesCnt_++;
}

// TODO: move to view module
void PerfProfilerSampling::mapPrint_()
{
    std::cout << std::endl;
    std::cout << "ip\t%"<< std::endl;
    for (auto i : this->map_)
    {
        std::cout << std::hex << "0x" << i.first << std::dec;
        std::cout << "\t";
        std::cout << (double)i.second / (double)this->samplesCnt_ * 100.0;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}