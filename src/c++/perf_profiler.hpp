#pragma once

#include <memory>


enum class IProfilerType { hwInstructions, sampling };


// This is a interface class
class IPerfProfiler
{
    protected:
        virtual ~IPerfProfiler() = default;
        IPerfProfiler& operator=(const IPerfProfiler&) = delete;

    public:
        virtual void run(int argc, char **argv) = 0;

        static auto createProfiler(IProfilerType type) -> std::shared_ptr<IPerfProfiler>;
};
