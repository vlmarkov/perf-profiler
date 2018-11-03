#pragma once

#include <vector>

#include <profiler.hpp>
#include <perf-event.hpp>


class PerfProfilerEvents : public IProfiler
{
    public:
        explicit PerfProfilerEvents();
        explicit PerfProfilerEvents(std::vector<uint32_t>& hw);
        ~PerfProfilerEvents() = default;

        void run(int argc, char **argv) override;

    private:
        std::vector<uint32_t> hw_;
        std::vector<uint64_t> hw_id_;
        std::vector<uint64_t> hw_val_;

        struct perf_event_attr pe_;

        void executeChild_(int argc, char **argv);
        void executeParent_(const pid_t childPid);
};
