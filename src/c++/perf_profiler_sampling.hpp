#pragma once

#include "perf_event.hpp"
#include "perf_profiler.hpp"

#include <map>


class PerfProfilerSampling : public IPerfProfiler
{
    private:
        struct perf_event_attr pe_;

        long samplesCnt_;

        std::map<uint64_t, int> map_;

        void mapPrint_();

        void executeChild_(int argc, char **argv);
        void executeParent_(const pid_t childPid);

        void pagePrint_(const perf_event::RecordPage& page);
        void samplePrint_(const perf_event::RecordSample& sample);

    public:
        explicit PerfProfilerSampling(uint32_t samplePeriod = 100000);
        ~PerfProfilerSampling() = default;

        void run(int argc, char **argv) override;
};
