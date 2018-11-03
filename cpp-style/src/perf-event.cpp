#include <iostream>

#include <perf-event.hpp>
#include <perf-exception.hpp>


PerfEvent::PerfEvent(struct perf_event_attr& pe, pid_t pid)
{
    this->fd_         = this->open_(pe, pid, -1, -1, 0);
    this->isGrouping_ = false;
}

PerfEvent::PerfEvent(struct perf_event_attr& pe, pid_t pid, std::vector<uint32_t>& events, std::vector<uint64_t>& ids)
{
    this->fd_ = this->groupInit_(pe, events[0], pid, -1, &ids[0]);

    for (size_t i = 1; i < events.size(); i++)
        this->groupInit_(pe,  events[i], pid, this->fd_, &ids[i]);

    this->isGrouping_ = true;
}

PerfEvent::~PerfEvent()
{
    this->close_();
}

// Start counting events or record sampling
void PerfEvent::start()
{
    if (::ioctl(this->fd_, PERF_EVENT_IOC_RESET, this->isGrouping_ ? PERF_IOC_FLAG_GROUP : 0) == -1)
        throw Exception("Failed to perf event: " + std::string(strerror(errno)), errno);

    if (::ioctl(this->fd_, PERF_EVENT_IOC_ENABLE, this->isGrouping_ ? PERF_IOC_FLAG_GROUP : 0) == -1)
        throw Exception("Failed to perf event::start() " + std::string(strerror(errno)), errno);
}

// Stop counting events or record sampling
void PerfEvent::stop()
{
    if (::ioctl(this->fd_, PERF_EVENT_IOC_DISABLE, this->isGrouping_ ? PERF_IOC_FLAG_GROUP : 0) == -1)
        throw Exception("Failed to stop(), " + std::string(strerror(errno)), errno);
}

int PerfEvent::getFd()
{
    return this->fd_;
}

int PerfEvent::groupInit_(
    struct perf_event_attr& pe,
    uint32_t          config,
    pid_t             pid,
    int               groupFd,
    uint64_t         *id)
{
    pe.config = config;
    int fd = this->open_(pe, pid, -1, groupFd, 0);

    if (::ioctl(fd, PERF_EVENT_IOC_ID, id) == -1)
        throw Exception("Failed to perf event: " + std::string(strerror(errno)), errno);

    return fd;
}

int PerfEvent::open_(struct perf_event_attr& pe, pid_t pid, int cpu, int groupFd, unsigned long flags)
{
    // Glibc does not provide a wrapper for this system call
    auto fd = ::syscall(__NR_perf_event_open, &pe, pid, cpu, groupFd, flags);
    if (fd < 0)
        throw Exception(std::string("Failed to perf_event_open(), not valid fd"));

    return fd;
}

void PerfEvent::close_()
{
    auto rc = ::close(this->fd_);
    if (rc != 0)
        throw Exception("Failed to close", rc);
}
