#include "perf_profiler_sampling.hpp"

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
            throw std::string("fork() failed");

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
    struct perf_event_mmap_page *mpage = nullptr;

    int fd = PerfEvent::open(this->pe_, childPid, -1, -1, 0);

    PerfEvent::mmap(this->pe_, fd, reinterpret_cast<void**>(&mpage));
  
    PerfEvent::start(fd, true);

    int status = 0;
    size_t prev_head = 0;
    uint64_t samplesCnt = 0;

    while (true)
    {
        pid_t waitPid = ::waitpid(childPid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (waitPid == -1)
            throw(std::string("waitpid(), error"));

        if (waitPid == 0)
        {
            // Child still running    
            if ((mpage->data_head != 0) && (mpage->data_head != prev_head))
            {
                prev_head = mpage->data_head;

                PerfProfilerSampling::ringBufferMetadataPrint_(mpage);
                PerfProfilerSampling::ringBufferDataPrint_(mpage);
                samplesCnt++;
            }
        }
        else if (waitPid == childPid)
        {
            // Child ended
            if (WIFEXITED(status))
            {
                printf("Samples gather: %d\n", (int)samplesCnt);
                printf("Child %d ended normally\n", childPid);
            }
            else if (WIFSIGNALED(status))
            {
                fprintf(stderr, "<%s> Child %d ended because of an uncaught signal\n", __FUNCTION__, childPid);
            }
            else if (WIFSTOPPED(status))
            {
                fprintf(stderr, "<%s> Child %d has stopped\n", __FUNCTION__, childPid);
            }
            break;
        }
    }

    PerfEvent::stop(fd, true);

    PerfEvent::close(fd);

    PerfEvent::unmmap(reinterpret_cast<void*>(mpage));
}

void PerfProfilerSampling::ringBufferMetadataPrint_(const struct perf_event_mmap_page *mpage)
{
    if (mpage == nullptr)
        throw std::string("Error");

    printf("The first metadata mmap page:\n");
    printf("\tversion        : %u\n",   mpage->version);
    printf("\tcompat version : %u\n",   mpage->compat_version);
    printf("\tlock           : %u\n",   mpage->lock);
    printf("\tindex          : %u\n",   mpage->index);
    printf("\toffset         : %llu\n", mpage->offset);
    printf("\ttime enabled   : %llu\n", mpage->time_enabled);
    printf("\ttime running   : %llu\n", mpage->time_running);
    printf("\tdata head      : %llu\n", mpage->data_head);
    printf("\tdata tail      : %llu\n", mpage->data_tail);
    printf("\tdata offset    : %llu\n", mpage->data_offset);
    printf("\tdata size     :  %llu\n", mpage->data_size);
    printf("\n");
}

void PerfProfilerSampling::ringBufferCopyGet_(
    const struct perf_event_mmap_page *mpage,
    const size_t                       bytes,
    void                              *dest)
{
    if (mpage == nullptr)
        throw std::string("Error");

    if (dest == nullptr)
        throw std::string("Error");

    uintptr_t base     = ((uintptr_t)mpage) + mpage->data_offset;
    size_t start_index = mpage->data_head - bytes;
    size_t end_index   = start_index + bytes;

    if (end_index <= mpage->data_size)
    {
        ::memcpy(dest, ((void *)(base + start_index)), bytes);
    }
}

void PerfProfilerSampling::ringBufferDataPrint_(const struct perf_event_mmap_page *mpage)
{
    if (mpage == nullptr)
        throw std::string("Error");

    uint8_t buf[4096] = { 0 };

    PerfProfilerSampling::ringBufferCopyGet_(mpage, sizeof(perf_record_sample_t), buf);

    PerfProfilerSampling::samplePrint_(buf);
}

void PerfProfilerSampling::samplePrint_(const void *buffer)
{
    if (buffer == nullptr)
        throw std::string("Error");

    perf_record_sample_t *sample = (perf_record_sample_t *)buffer;

    if (sample->header.type == PERF_RECORD_SAMPLE)
    {
        printf("Sample's header:\n");
        printf("\ttype : PERF_RECORD_SAMPLE\n");
        printf("\tmisc : %d\n",   sample->header.misc);
        printf("\tsize : %d\n",   sample->header.size);
        printf("\n");

        printf("Sample's data:\n");
        printf("\ttid  : %d\n",   sample->tid);
        printf("\tpid  : %d\n",   sample->pid);
        printf("\tip   : 0x%lx\n", sample->ip);
        printf("\n");
    }
}
