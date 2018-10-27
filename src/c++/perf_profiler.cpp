#include "perf_profiler.hpp"
#include "perf_profiler_events.hpp"
#include "perf_profiler_sampling.hpp"


smartPtr IPerfProfiler::createProfiler(IProfilerType type)
{
    switch (type)
    {
        case IProfilerType::hwInstructions:
            return std::make_shared<PerfProfilerEvents>();
        case IProfilerType::sampling:
            return std::make_shared<PerfProfilerSampling>();
    }
}
