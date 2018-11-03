#include <iostream>
#include <exception>

#include <perf-event.hpp>
#include <perf-profiler.hpp>
#include <perf-exception.hpp>
#include <perf-profiler-events.hpp>
#include <perf-profiler-sampling.hpp>


void test1(int argc, char **argv)
{
    auto profiler = IPerfProfiler::createProfiler(IProfilerType::hwInstruction);
    profiler->run(argc, argv);
}

void test2(int argc, char **argv)
{
    auto profiler = IPerfProfiler::createProfiler(IProfilerType::hwCache);
    profiler->run(argc, argv);
}

void test3(int argc, char **argv)
{
    auto profiler = IPerfProfiler::createProfiler(IProfilerType::hwBranch);
    profiler->run(argc, argv);
}

void test4(int argc, char **argv)
{
    auto profiler = IPerfProfiler::createProfiler(IProfilerType::sampling);
    profiler->run(argc, argv);
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

        test3(argc, argv);

        test4(argc, argv);
    }

    return 0;
}
