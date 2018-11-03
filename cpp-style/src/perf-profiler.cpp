#include <perf-profiler.hpp>
#include <perf-exception.hpp>
#include <perf-profiler-events.hpp>
#include <perf-profiler-sampling.hpp>


auto IPerfProfiler::createProfiler(IProfilerType type) -> std::shared_ptr<IPerfProfiler>
{
    switch (type)
    {
        case IProfilerType::hwInstruction:
        {
            std::vector<uint32_t> hw_cpu = {
                PERF_COUNT_HW_CPU_CYCLES,
                PERF_COUNT_HW_INSTRUCTIONS
            };
            
            return std::make_shared<PerfProfilerEvents>(hw_cpu);
        }
        case IProfilerType::hwCache:
        {
            std::vector<uint32_t> hw_cache = {
                PERF_COUNT_HW_CACHE_REFERENCES,
                PERF_COUNT_HW_CACHE_MISSES
            };      

            return std::make_shared<PerfProfilerEvents>(hw_cache);
        }
        case IProfilerType::hwBranch:
        {
            std::vector<uint32_t> hw_branch = {
                PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
                PERF_COUNT_HW_BRANCH_MISSES
            };

            return std::make_shared<PerfProfilerEvents>(hw_branch);
        }
        case IProfilerType::sampling:
        {
            return std::make_shared<PerfProfilerSampling>();
        }
    }

    throw Exception("IPerfProfiler::createProfiler(), not supproted type");
}
