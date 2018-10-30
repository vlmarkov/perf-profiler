#include <iostream>
#include <exception>

#include "perf_event.hpp"
#include "perf_profiler.hpp"
#include "perf_profiler_events.hpp"
#include "perf_profiler_sampling.hpp"
#include "perf_profiler_exception.hpp"


void test1(int argc, char **argv)
{
    try
    {
        auto profiler = IPerfProfiler::createProfiler(IProfilerType::hwInstructions);
        profiler->run(argc, argv);
    }
    catch(PerfProfilerException& exception)
    {
        std::cerr << exception.what() << " : " << exception.code() << std::endl;
    }
    catch(std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
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
    catch(PerfProfilerException& exception)
    {
        std::cerr << exception.what() << " : " << exception.code() << std::endl;
    }
    catch(std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unexpected exception" << std::endl;
        return;
    }
}

static void usage(char const *argv0)
{
    fprintf(stderr, "usage: %s <your-test-program>\n", argv0);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv[0]);
    }
    else
    {
        test1(argc, argv);
        test2(argc, argv);
    }

    return 0;
}
