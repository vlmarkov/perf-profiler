#pragma once

#include "perf_event.hpp"
#include "perf_profiler.hpp"


class PerfProfilerSampling : public IPerfProfiler
{
    public:
        explicit PerfProfilerSampling(uint32_t samplePeriod = 100000);
        ~PerfProfilerSampling() = default;

        void run(int argc, char **argv) override;

    private:
        void executeChild_(int argc, char **argv);
        void executeParent_(const pid_t childPid);

        void pagePrint_(const PerfEvent::RecordPage& page);
        void samplePrint_(const PerfEvent::RecordSample& sample);

        struct perf_event_attr pe_;
};
