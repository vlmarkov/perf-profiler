#pragma once

#include <vector>

#include <perf-event.hpp>
#include <perf-profiler.hpp>


class PerfProfilerEvents : public IPerfProfiler
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
