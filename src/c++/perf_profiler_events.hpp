#pragma once

#include "perf_event.hpp"
#include "perf_profiler.hpp"


class PerfProfilerEvents : public IPerfProfiler
{
    private:
        struct perf_event_attr pe_;

        void executeChild_(int argc, char **argv);
        void executeParent_(const pid_t childPid);

    public:
        explicit PerfProfilerEvents();
        ~PerfProfilerEvents() = default;

        void run(int argc, char **argv) override;
};
