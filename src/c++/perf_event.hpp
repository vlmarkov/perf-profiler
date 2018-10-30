#pragma once

#include <string>

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <linux/perf_event.h>


// This is a wrappers for a 'perf_event_open'
// See for more details http://man7.org/linux/man-pages/man2/perf_event_open.2.html
namespace perf_event
{
    int open(struct perf_event_attr& pe, pid_t pid, int cpu, int groupFd, unsigned long flags);
    void close(int fd);

    void start(int fd, bool isGrouping);
    void stop(int fd, bool isGrouping);

    class RecordPage
    {
        public:
            RecordPage()  = default;
            ~RecordPage() = default;

        struct perf_event_mmap_page mpage;
    };

    class RecordSample
    {
        public:
            RecordSample()  = default;
            ~RecordSample() = default;

            struct perf_event_header header;
            uint64_t                 ip;
            uint32_t                 pid;
            uint32_t                 tid;
    };

    class RingBuffer
    {
        public:
            RingBuffer(int fd);
            ~RingBuffer();

            bool hasData();

            RecordPage   pageGet();
            RecordSample sampleGet();

        private:
            struct perf_event_mmap_page* mpage_;
            size_t prevHead_;

            unsigned mmapSizeGet_();
    };

    // TODO
    typedef struct read_format
    {
        uint64_t nr;
        struct {
            uint64_t value;
            uint64_t id;
        } values[];
    } read_format_t;
}
