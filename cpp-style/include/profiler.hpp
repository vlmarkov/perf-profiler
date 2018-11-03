#pragma once


// This is a interface class
class IProfiler
{
    protected:
        virtual ~IProfiler() = default;
        IProfiler& operator=(const IProfiler&) = delete;

    public:
        virtual void run(int argc, char **argv) = 0;
};
