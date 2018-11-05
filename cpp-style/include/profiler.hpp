#pragma once

#include <string>


// This is a interface class
class IProfiler
{
    protected:
        virtual ~IProfiler() = default;
        IProfiler& operator=(const IProfiler&) = delete;

    public:
        virtual void run(std::string& args) = 0;
};
