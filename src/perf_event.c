#define _GNU_SOURCE

#include <sys/mman.h>
#include <inttypes.h>

#include "perf_event.h"


/*****************************************************************************/
/* Perf-event: common API                                                    */
/*****************************************************************************/
long perf_event_open(
    struct perf_event_attr *pe,
    const pid_t             pid,
    const int               cpu,
    const int               group_fd,
    const unsigned long     flags)
{
    return syscall(__NR_perf_event_open, pe, pid, cpu, group_fd, flags);
}

static unsigned perf_event_mmap_size(void)
{
    const int buf_size_shift = 8;
    return ((1U << buf_size_shift) + 1) * sysconf(_SC_PAGESIZE);
}

// Release resources
int perf_event_close(const int fd, void *mmapped)
{
    if (fd < 0)
    {
        fprintf(stderr, "Failed to close perf event, not valid fd\n");
        return -1;        
    }

    if (close(fd) != 0)
    {
        fprintf(stderr, "Failed to close perf event, error in close()\n");
        return -1;           
    }

    if (mmapped)
    {
        munmap(mmapped, perf_event_mmap_size());
    }

    return 0;
}

// Start counting events
int perf_event_start(const int fd, const bool grouping)
{
    if (fd < 0)
    {
        fprintf(stderr, "Failed to start perf event, not valid fd\n");
        return -1;
    }
    
    if (ioctl(fd, PERF_EVENT_IOC_RESET, grouping ? PERF_IOC_FLAG_GROUP : 0) == -1)
    {
        fprintf(stderr, "Failed to start perf event: %s", strerror(errno));
        return -1;
    }

    if (ioctl(fd, PERF_EVENT_IOC_ENABLE, grouping ? PERF_IOC_FLAG_GROUP : 0) == -1)
    {
        fprintf(stderr, "Failed to start perf event: %s", strerror(errno));
        return -1;
    }

    return 0;
}

// Stop counting events
int perf_event_stop(const int fd, const bool grouping)
{
    if (fd < 0)
    {
        fprintf(stderr, "Failed to stop perf event, not valid fd\n");
        return -1;
    }
    
    if (ioctl(fd, PERF_EVENT_IOC_DISABLE, grouping ? PERF_IOC_FLAG_GROUP : 0) == -1)
    {
        fprintf(stderr, "Failed to stop perf event: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int perf_event_mmap(const struct perf_event_attr *pe, const int fd, void **mmapped)
{
    // If sampling, map the perf event file
    if (pe->sample_type != 0 && pe->sample_period != 0)
    {
        void *ring_buffer = mmap(NULL,
                                 perf_event_mmap_size(),
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd,
                                 0);

        if (ring_buffer == MAP_FAILED)
        {
            fprintf(stderr, "Failed to mmap perf event file\n");
            return -1;
        }

        *mmapped = (struct perf_event_mmap_page *)ring_buffer;
    }

    return 0;
}


/*****************************************************************************/
/* Perf-event: event API                                                     */
/*****************************************************************************/
int perf_event_read_instructions(const int fd, long long *instructions)
{
    if (fd < 0)
    {
        fprintf(stderr, "Failed to read perf event instructions, not valid fd\n");
        return -1;
    }

    if (!instructions)
    {
        fprintf(stderr, "Failed to read perf event instructions, not valid pointer\n");
        return -1;
    }

    if (read(fd, instructions, sizeof(long long)) != sizeof(long long))
    {
        fprintf(stderr, "Failed to read perf event instructions, error in read()\n");
        return -1;        
    }

    return 0;
}

int perf_event_init_instructions(struct perf_event_attr *pe)
{
    if (!pe)
    {
        fprintf(stderr, "Failed to read perf init instructions, not valid pointer\n");
        return -1;
    }

    memset(pe, 0, sizeof(struct perf_event_attr));

    pe->type           = PERF_TYPE_HARDWARE;
    pe->size           = sizeof(struct perf_event_attr);
    pe->config         = PERF_COUNT_HW_INSTRUCTIONS;
    pe->disabled       = 1;
    pe->exclude_kernel = 1;
    pe->exclude_hv     = 1;

    return 0;
}

int perf_events_read(const int fd, const uint64_t *ids, uint64_t *vals)
{
    if (fd < 0)
        return -1;

    if (!ids)
        return -1;

    if (!vals)
        return -1;


    char buf[4096] = { 0 };
    read_format_t *rf = (read_format_t *)buf;

    read(fd, buf, sizeof(buf));

    for (int i = 0; i < rf->nr; i++)
    {
        for (int j = 0; j < rf->nr; j++)
        {
           if (rf->values[i].id == ids[j])
            {
                vals[j] = rf->values[i].value;
                break;
            }
        }
    }

    return 0;
}

void perf_events_hw_show(const uint64_t *vals)
{
    if (!vals)
        return;

    printf("PERF_COUNT_HW_CPU_CYCLES          : %" PRIu64 "\n", vals[0] ? vals[0] : -1);
    printf("PERF_COUNT_HW_INSTRUCTIONS        : %" PRIu64 "\n", vals[1] ? vals[1] : -1);
    printf("PERF_COUNT_HW_CACHE_REFERENCES    : %" PRIu64 "\n", vals[2] ? vals[2] : -1);
    printf("PERF_COUNT_HW_CACHE_MISSES        : %" PRIu64 "\n", vals[3] ? vals[3] : -1);
    printf("PERF_COUNT_HW_BRANCH_INSTRUCTIONS : %" PRIu64 "\n", vals[4] ? vals[4] : -1);
    printf("PERF_COUNT_HW_BRANCH_MISSES       : %" PRIu64 "\n", vals[5] ? vals[5] : -1);
}

static int perf_event_init_(
    struct perf_event_attr *pe,
    const uint32_t          type,
    const uint32_t          config,
    const pid_t             pid,
    const int               group_fd,
    uint64_t               *id)
{
    if (!pe)
        return -1;

    memset(pe, 0, sizeof(struct perf_event_attr));

    pe->type           = type;
    pe->size           = sizeof(struct perf_event_attr);
    pe->config         = config;
    pe->disabled       = 1;
    pe->exclude_kernel = 1;
    pe->exclude_hv     = 1;
    pe->read_format    = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    int fd = perf_event_open(pe, pid, -1, group_fd, 0);
    if (fd == -1)
        return -1;

    ioctl(fd, PERF_EVENT_IOC_ID, id);

    return fd;
}

int perf_events_init(
    const pid_t     pid,
    const uint32_t  type,
    const size_t    size,
    const uint32_t *events,
    uint64_t       *ids)
{
    if (!events)
        return -1;

    if (!ids)
        return -1;

    struct perf_event_attr pe;

    int fd = perf_event_init_(&pe, type, events[0], pid, -1, &ids[0]);
    if (fd == -1)
    {
        fprintf(stderr, "<%s> Can't init perf-event[%d]\n", __FUNCTION__, 0);
        return -1;
    }

    for (size_t i = 1; i < size; i++)
    {
        if (perf_event_init_(&pe, type, events[i], pid, fd, &ids[i]) == -1)
        {
            fprintf(stderr, "<%s> Can't init perf-event[%d]\n", __FUNCTION__, (int)i);
            return -1;
        }
    }

    return fd;
}


/*****************************************************************************/
/* Perf-event: sampling API                                                  */
/*****************************************************************************/
int perf_event_init_sampling(struct perf_event_attr *pe)
{
    if (!pe)
        return -1;

    memset(pe, 0, sizeof(struct perf_event_attr));

    pe->type           = PERF_TYPE_SOFTWARE;
    pe->config         = PERF_COUNT_SW_TASK_CLOCK;
    pe->size           = sizeof(struct perf_event_attr);
    pe->disabled       = 1;
    pe->exclude_kernel = 1;
    pe->exclude_hv     = 1;
    pe->read_format    = PERF_FORMAT_ID;
    pe->sample_period  = SAMPLE_PERIOD;
    pe->sample_id_all  = 1;
    pe->sample_type    = PERF_SAMPLE_IP | PERF_SAMPLE_TID;

    return 0;
}

void perf_event_ring_buffer_copy_get(const struct perf_event_mmap_page *mpage,
                                     const size_t                       bytes,
                                     void                              *dest)
{
    if (!mpage)
        return;

    if (!dest)
        return;

    uintptr_t base     = ((uintptr_t)mpage) + mpage->data_offset;
    size_t start_index = mpage->data_head - bytes;
    size_t end_index   = start_index + bytes;

    if (end_index <= mpage->data_size)
    {
        memcpy(dest, ((void *)(base + start_index)), bytes);
    }
}

void perf_event_ring_buffer_metadata_print(const struct perf_event_mmap_page *mpage)
{
    if (!mpage)
        return;

    printf("The first metadata mmap page:\n");
    printf("\tversion        : %u\n",   mpage->version);
    printf("\tcompat version : %u\n",   mpage->compat_version);
    printf("\tlock           : %u\n",   mpage->lock);
    printf("\tindex          : %u\n",   mpage->index);
    printf("\toffset         : %llu\n", mpage->offset);
    printf("\ttime enabled   : %llu\n", mpage->time_enabled);
    printf("\ttime running   : %llu\n", mpage->time_running);
    printf("\tdata head      : %llu\n", mpage->data_head);
    printf("\tdata tail      : %llu\n", mpage->data_tail);
    printf("\tdata offset    : %llu\n", mpage->data_offset);
    printf("\tdata size     :  %llu\n", mpage->data_size);
    printf("\n");
}

void perf_event_ring_buffer_data_print(const struct perf_event_mmap_page *mpage,
                                       const size_t                       size,
                                       perf_event_cb_print_t              cb_print)
{
    if (!mpage)
        return;

    uint8_t buf[4096] = { 0 };

    perf_event_ring_buffer_copy_get(mpage, size, buf);

    cb_print(buf);
}

void perf_event_samples_print(const void *buffer)
{
    if (!buffer)
        return;

    perf_record_sample_t *sample = (perf_record_sample_t *)buffer;

    if (sample->header.type == PERF_RECORD_SAMPLE)
    {
        printf("Sample's header:\n");
        printf("\ttype : PERF_RECORD_SAMPLE\n");
        printf("\tmisc : %d\n",   sample->header.misc);
        printf("\tsize : %d\n",   sample->header.size);
        printf("\n");

        printf("Sample's data:\n");
        printf("\ttid  : %d\n",   sample->tid);
        printf("\tpid  : %d\n",   sample->pid);
        printf("\tip   : 0x%lx\n", sample->ip);
        printf("\n");
    }
}
