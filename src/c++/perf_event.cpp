#include "perf_event.hpp"


int PerfEvent::open(struct perf_event_attr& pe, pid_t pid, int cpu, int groupFd, unsigned long flags)
{
    // Glibc does not provide a wrapper for this system call
    int fd = ::syscall(__NR_perf_event_open, &pe, pid, cpu, groupFd, flags);
    if (fd < 0)
        throw std::string("Failed to open perf event, not valid fd"); // TODO: class Exception

    return fd;
}

// Release resources
void PerfEvent::close(int fd)
{
    if (fd < 0)
        throw std::string("Failed to close perf event, not valid fd");  // TODO: class Exception

    if (::close(fd) != 0)
        throw std::string("Failed to close perf event, error in close()"); // TODO: class Exception
}

// Start counting events or record sampling
void PerfEvent::start(int fd, bool isGrouping)
{
    if (fd < 0)
        throw std::string("Failed to start perf event, not valid fd");  // TODO: class Exception
    
    if (::ioctl(fd, PERF_EVENT_IOC_RESET, isGrouping ? PERF_IOC_FLAG_GROUP : 0) == -1)
        throw std::string("Failed to start perf event: " + std::string(strerror(errno)));  // TODO: class Exception

    if (::ioctl(fd, PERF_EVENT_IOC_ENABLE, isGrouping ? PERF_IOC_FLAG_GROUP : 0) == -1)
        throw std::string("Failed to start perf event: " + std::string(strerror(errno)));  // TODO: class Exception
}

// Stop counting events or record sampling
void PerfEvent::stop(int fd, bool isGrouping)
{
    if (fd < 0)
        throw std::string("Failed to stop perf event, not valid fd");  // TODO: class Exception
    
    if (::ioctl(fd, PERF_EVENT_IOC_DISABLE, isGrouping ? PERF_IOC_FLAG_GROUP : 0) == -1)
        throw std::string("Failed to stop perf event: " + std::string(strerror(errno)));  // TODO: class Exception
}

void PerfEvent::mmap(struct perf_event_attr& pe, int fd, void **mmapped)
{
    if (fd < 0)
        throw std::string("Failed to mmap perf event, not valid fd");  // TODO: class Exception

    if (pe.sample_type == 0 || pe.sample_period == 0)
        throw std::string("Failed to mmap perf event, not valid params");  // TODO: class Exception

    void *ring_buffer = ::mmap(NULL, PerfEvent::mmapSize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ring_buffer == MAP_FAILED)
        throw std::string("Failed to mmap perf event file");  // TODO: class Exception

    *mmapped = (struct perf_event_mmap_page *)ring_buffer;
}

// Release resources
void PerfEvent::unmmap(void *mmapped)
{
    if (mmapped == nullptr)
        throw std::string("Failed to close perf event, not valid ptr"); // TODO: class Exception

    ::munmap(mmapped, PerfEvent::mmapSize()); // TODO: class Exception
}

unsigned PerfEvent::mmapSize()
{
    return ((1U << 8) + 1) * ::sysconf(_SC_PAGESIZE);
}
