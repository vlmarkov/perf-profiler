#include "perf_profiler_exception.hpp"


PerfProfilerException::PerfProfilerException(const std::string& err, const int code) : 
code_(code), err_(err)
{
    ;
}

const char* PerfProfilerException::what() const noexcept
{
    return err_.c_str();
}

const int PerfProfilerException::code() const noexcept
{
    return code_;
}

