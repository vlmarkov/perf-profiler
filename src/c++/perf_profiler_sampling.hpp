#pragma once

#include "perf_event.hpp"
#include "perf_profiler.hpp"


typedef struct
{
    struct perf_event_header header;
    uint64_t                 ip;
    uint32_t                 pid;
    uint32_t                 tid;
} perf_record_sample_t;


class PerfProfilerSampling : public IPerfProfiler
{
    public:
        explicit PerfProfilerSampling(uint32_t samplePeriod = 100000);
        ~PerfProfilerSampling() = default;

        void run(int argc, char **argv) override;

    private:
        void executeChild_(int argc, char **argv);
        void executeParent_(const pid_t childPid);

        void ringBufferMetadataPrint_(const struct perf_event_mmap_page *mpage);
        void ringBufferCopyGet_(const struct perf_event_mmap_page *mpage, const size_t bytes, void *dest);
        void ringBufferDataPrint_(const struct perf_event_mmap_page *mpage);
        void samplePrint_(const void *buffer);

        struct perf_event_attr pe_;
};
