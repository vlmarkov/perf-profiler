#include <memory>
#include <iostream>

#include <factory.hpp>
#include <perf-profiler-factory.hpp>


void test1(int argc, char **argv)
{
    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createInstructionProfiler(); 

    profiler->run(argc, argv);
}

void test2(int argc, char **argv)
{
    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createCacheProfiler();

    profiler->run(argc, argv);
}

void test3(int argc, char **argv)
{
    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createBranchProfiler();

    profiler->run(argc, argv);
}

void test4(int argc, char **argv)
{
    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createSamplingProfiler();

    profiler->run(argc, argv);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " <your-test-program>" << std::endl;
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
