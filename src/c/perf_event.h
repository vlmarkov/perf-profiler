#pragma once

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <signal.h>
#include <stddef.h>


#define SAMPLE_PERIOD 1000000 // Time between samples

typedef void (*perf_event_cb_print_t)(const void *buf);

typedef struct read_format
{
    uint64_t nr;
    struct {
        uint64_t value;
        uint64_t id;
    } values[];
} read_format_t;

typedef struct
{
    struct perf_event_header header;
    uint64_t                 ip;
    uint32_t                 pid;
    uint32_t                 tid;
} perf_record_sample_t;


/*****************************************************************************/
/* Perf-event: common API                                                    */
/*****************************************************************************/
long perf_event_open(struct perf_event_attr *pe, const pid_t pid, const int cpu, const int group_fd, const unsigned long flags);
int perf_event_close(const int fd, void *mmapped);
int perf_event_start(const int fd, const bool grouping);
int perf_event_stop(const int fd, const bool grouping);
int perf_event_mmap(const struct perf_event_attr *pe, const int fd, void **mmapped);


/*****************************************************************************/
/* Perf-event: event API                                                     */
/*****************************************************************************/
int perf_event_read_instructions(const int fd, long long *instructions);
int perf_event_init_instructions(struct perf_event_attr *pe);
int perf_events_read(const int fd, const uint64_t *ids, uint64_t *vals);
void perf_events_hw_show(const uint64_t *vals);
int perf_events_init(const pid_t pid, const uint32_t type, const size_t size, const uint32_t *events, uint64_t *ids);


/*****************************************************************************/
/* Perf-event: sampling API                                                  */
/*****************************************************************************/
int perf_event_init_sampling(struct perf_event_attr *pe);
void perf_event_ring_buffer_copy_get(const struct perf_event_mmap_page *mpage, const size_t bytes, void *dest);
void perf_event_ring_buffer_metadata_print(const struct perf_event_mmap_page *mpage);
void perf_event_ring_buffer_data_print(const struct perf_event_mmap_page *mpage, const size_t size, perf_event_cb_print_t cb);
void perf_event_samples_print(const void *buffer);
