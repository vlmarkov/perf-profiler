#pragma once

#include "perf_event.hpp"
#include "perf_profiler.hpp"


typedef struct read_format
{
    uint64_t nr;
    struct {
        uint64_t value;
        uint64_t id;
    } values[];
} read_format_t;


class PerfProfilerEvents : public IPerfProfiler
{
    public:
        explicit PerfProfilerEvents();
        ~PerfProfilerEvents() = default;

        void run(int argc, char **argv) override;

    private:
        void executeChild_(int argc, char **argv);
        void executeParent_(const pid_t childPid);

        struct perf_event_attr pe_;
};
