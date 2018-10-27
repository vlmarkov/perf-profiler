#include "perf_event.hpp"


// Acquire resources
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


namespace PerfEvent
{
    RingBuffer::RingBuffer(int fd)
    {
        if (fd < 0)
            throw std::string("Failed to mmap perf event, not valid fd"); // TODO: class Exception

        void *mmap = ::mmap(NULL, RingBuffer::mmapSizeGet_(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (mmap == MAP_FAILED)
            throw std::string("Failed to mmap perf event file"); // TODO: class Exception

        this->mpage_    = static_cast<struct perf_event_mmap_page*>(mmap);
        this->prevHead_ = 0;
    }

    RingBuffer::~RingBuffer()
    {
        ::munmap(static_cast<void*>(this->mpage_), RingBuffer::mmapSizeGet_());
    }

    bool RingBuffer::hasData()
    {
        if ((this->mpage_->data_head != 0) &&
            (this->mpage_->data_head != this->prevHead_))
        {
            this->prevHead_ = this->mpage_->data_head;
            return true;
        }
        return false;
    }

    RecordPage RingBuffer::pageGet()
    {
        const auto bytes = sizeof(RecordPage);

        RecordPage recordPage;

        ::memcpy(&recordPage, reinterpret_cast<void*>(this->mpage_), bytes);

        return recordPage;
    }

    RecordSample RingBuffer::sampleGet()
    {
        const uintptr_t base = reinterpret_cast<uintptr_t>(this->mpage_) + this->mpage_->data_offset;
        const auto bytes     = sizeof(RecordSample);
        const auto startIdx  = this->mpage_->data_head - bytes;
        const auto endIdx    = startIdx + bytes;

        RecordSample recordSample;

        if (endIdx <= this->mpage_->data_size)
        {
            ::memcpy(&recordSample, (reinterpret_cast<void*>(base + startIdx)), bytes);
        }
        else
        {
            // TODO: is this possible at all?
        }

        return recordSample;
    }

    unsigned RingBuffer::mmapSizeGet_()
    {
        return ((1U << 8) + 1) * ::sysconf(_SC_PAGESIZE);
    }
}
