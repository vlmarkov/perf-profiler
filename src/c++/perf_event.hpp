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
namespace PerfEvent
{
    int open(struct perf_event_attr& pe, pid_t pid, int cpu, int groupFd, unsigned long flags);
    void close(int fd);

    void start(int fd, bool isGrouping);
    void stop(int fd, bool isGrouping);

    void mmap(struct perf_event_attr& pe, int fd, void **mmapped);
    void unmmap(void *mmapped);

    unsigned mmapSize();
}
