#pragma once

#include <string>
#include <exception>


class PerfProfilerException : public std::exception
{
    private:
        int code_;
        std::string err_;
    
    public:
        PerfProfilerException(const std::string& err, const int code = 0);
        ~PerfProfilerException() = default;

        virtual const char* what() const noexcept override;
        const int code() const noexcept;
};
