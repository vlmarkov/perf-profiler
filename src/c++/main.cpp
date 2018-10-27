#include <iostream>

#include "perf_event.hpp"
#include "perf_profiler.hpp"
#include "perf_profiler_events.hpp"
#include "perf_profiler_sampling.hpp"


void test1(int argc, char **argv)
{
    try
    {
        auto profiler = IPerfProfiler::createProfiler(IProfilerType::hwInstructions);
        profiler->run(argc, argv);
    }
    catch (std::string& exception)
    {
        std::cerr << exception << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unexpected exception" << std::endl;
        return;
    }
}

void test2(int argc, char **argv)
{
    try
    {
        auto profiler = IPerfProfiler::createProfiler(IProfilerType::sampling);
        profiler->run(argc, argv);
    }
    catch (std::string& exception)
    {
        std::cerr << exception << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unexpected exception" << std::endl;
        return;
    }
}

int main(int argc, char **argv)
{
    test1(argc, argv);

    test2(argc, argv);

    return 0;
}
