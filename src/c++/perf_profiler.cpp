#include "perf_profiler.hpp"
#include "perf_profiler_events.hpp"
#include "perf_profiler_sampling.hpp"
#include "perf_profiler_exception.hpp"


auto IPerfProfiler::createProfiler(IProfilerType type) -> std::shared_ptr<IPerfProfiler>
{
    switch (type)
    {
        case IProfilerType::hwInstructions:
            return std::make_shared<PerfProfilerEvents>();
        case IProfilerType::sampling:
            return std::make_shared<PerfProfilerSampling>();
    }

    throw PerfProfilerException("IPerfProfiler::createProfiler(), not supproted type");
}
