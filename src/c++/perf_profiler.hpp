#pragma once

#include <memory>


enum class IProfilerType { hwInstructions, sampling };

class IPerfProfiler;
using smartPtr = std::shared_ptr<IPerfProfiler>;

// This is a interface class
class IPerfProfiler
{
    protected:
        virtual ~IPerfProfiler() = default;
        IPerfProfiler& operator=(const IPerfProfiler&) = delete;   // Do not allow assignment

    public:
        virtual void run(int argc, char **argv) = 0;               // Pure virtual method

        static smartPtr createProfiler(IProfilerType type);        // Fabric method
};
