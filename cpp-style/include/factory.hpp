#pragma once

#include <memory>

#include <profiler.hpp>


// This is a interface class
class IFactory
{
    protected:
        virtual ~IFactory() = default;
        IFactory& operator=(const IFactory&) = delete;

    public: 
        virtual std::shared_ptr<IProfiler> createCacheProfiler()       = 0;
        virtual std::shared_ptr<IProfiler> createBranchProfiler()      = 0;
        virtual std::shared_ptr<IProfiler> createInstructionProfiler() = 0;
        virtual std::shared_ptr<IProfiler> createSamplingProfiler()    = 0;
};
