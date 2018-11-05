#include <memory>
#include <string>
#include <iostream>

#include <factory.hpp>
#include <perf-profiler-factory.hpp>


void test1(std::string& args)
{
    std::cout << "Running test: hw events insturtions" << std::endl;

    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createInstructionProfiler(); 

    profiler->run(args);
}

void test2(std::string& args)
{
    std::cout << "Running test: hw events cache" << std::endl;

    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createCacheProfiler();

    profiler->run(args);
}

void test3(std::string& args)
{
    std::cout << "Running test: hw events branch" << std::endl;

    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createBranchProfiler();

    profiler->run(args);
}

void test4(std::string& args)
{
    std::cout << "Running test: sampling" << std::endl;

    std::shared_ptr<IFactory> factory = std::make_shared<PerfProfilerFactory>();

    auto profiler = factory->createSamplingProfiler();

    profiler->run(args);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " <your-test-program>" << std::endl;
    }
    else
    {
        std::string args;

        for (auto i = 1; i < argc; ++i)
        {
            args += std::string(argv[i]);
            args += std::string(" ");
        }

        test1(args);
        test2(args);
        test3(args);
        test4(args);
    }

    return 0;
}
