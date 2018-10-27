#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "perf_event.h"

/*****************************************************************************/
/* Local prototypes                                                          */
/*****************************************************************************/
static void usage(char const *argv0);
static void fork_and_exec_fail(const char *error);
static void fork_and_exec(int argc, char **argv);
static void exec_child(int argc, char **argv);
static void exec_parent(const pid_t child_pid);


/*****************************************************************************/
/* This funtion used to:                                                     */
/* - print help message                                                      */
/*****************************************************************************/
static void usage(char const *argv0)
{
    fprintf(stderr, "usage: %s <your-test-program>\n", argv0);
}

/*****************************************************************************/
/* This funtion used to:                                                     */
/* - print error message                                                     */
/* - abort execution                                                         */
/*****************************************************************************/
static void fork_and_exec_fail(const char *error)
{
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
}

/*****************************************************************************/
/* This funtion used to:                                                     */
/* - execute child testing program                                           */
/*****************************************************************************/
static void exec_child(int argc, char **argv)
{
    char *args[] = { argv[1], NULL };
    execve(argv[1], args, NULL);

    // Never reach
    _exit(EXIT_SUCCESS);
}

/*****************************************************************************/
/* This funtion used to:                                                     */
/* - monitor child                                                           */
/* - init perf-event                                                         */
/* - gather samples                                                          */
/*****************************************************************************/
static void exec_parent(const pid_t child_pid)
{
    struct perf_event_mmap_page *mpage = NULL;
    struct perf_event_attr pe;

    if (perf_event_init_sampling(&pe) != 0)
        fork_and_exec_fail("Can't init perf-event sampling");

    int fd = perf_event_open(&pe, child_pid, -1, -1, 0);
    if (fd < 0)
        fork_and_exec_fail("Can't open perf-event fd");

    if (perf_event_mmap(&pe, fd, (void *)&mpage) != 0)
        fork_and_exec_fail("Can't mmap perf-event");

    if (perf_event_start(fd, false) != 0)
        fork_and_exec_fail("Can't start perf-event");

    int status = 0;
    size_t prev_head = 0;
    uint32_t samples_cnt = 0;

    while (true)
    {
        pid_t wait_pid = waitpid(child_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (wait_pid == -1)
            fork_and_exec_fail("Can't waitpid()!"); // TODO: error string

        if (wait_pid == 0)
        {
            // Child still running
            if ((mpage->data_head != 0) && (mpage->data_head != prev_head))
            {
                prev_head = mpage->data_head;

                perf_event_ring_buffer_metadata_print(mpage);
                perf_event_ring_buffer_data_print(mpage,
                                                  sizeof(perf_record_sample_t),
                                                  perf_event_samples_print);
                samples_cnt++;
            }

        }
        else if (wait_pid == child_pid)
        {
            // Child ended
            if (WIFEXITED(status))
            {
                printf("Samples gather: %d\n", samples_cnt);
                printf("Child %d ended normally\n", child_pid);
            }
            else if (WIFSIGNALED(status))
            {
                printf("Child %d ended because of an uncaught signal\n",child_pid);
            }
            else if (WIFSTOPPED(status))
            {
                printf("Child %d has stopped\n", child_pid);
            }
            break;
        }
    }

    if (perf_event_stop(fd, false) != 0)
        fork_and_exec_fail("Can't stop perf-event!");

    if (perf_event_close(fd, (void *)mpage) != 0)
        fork_and_exec_fail("Can't close perf-event fd!");
}

/*****************************************************************************/
/* This funtion used to:                                                     */
/* - fork and exec child                                                     */
/* - monitor child by parent (gather samples)                                */
/*****************************************************************************/
static void fork_and_exec(int argc, char **argv)
{
    pid_t child_pid = 0;

    switch (child_pid = fork())
    {
        case -1:
            fork_and_exec_fail("fork() failed"); // TODO: error message
            break;

        case 0:
            exec_child(argc, argv);
            break;

        default: 
            exec_parent(child_pid);
            break;
    }
}

/*****************************************************************************/
/* This funtion used to:                                                     */
/* - run perf-event sampling profiller                                       */
/*****************************************************************************/
int main(int argc, char **argv)
{
    if (argc < 2)
        usage(argv[0]);
    else
        fork_and_exec(argc, argv);

    return 0;
}
