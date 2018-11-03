#include <factory.hpp>
#include <profiler.hpp>


class PerfProfilerFactory : public IFactory
{
    public:
        PerfProfilerFactory()  = default;
        ~PerfProfilerFactory() = default;

        std::shared_ptr<IProfiler> createCacheProfiler()       override;
        std::shared_ptr<IProfiler> createBranchProfiler()      override;
        std::shared_ptr<IProfiler> createInstructionProfiler() override;
        std::shared_ptr<IProfiler> createSamplingProfiler()    override;
};
