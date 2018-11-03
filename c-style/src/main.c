#include <stdio.h>

#include <profiler-hw-events.h>
#include <profiler-sampling.h>


/*****************************************************************************/
/* This funtion used to:                                                     */
/* - print help message                                                      */
/*****************************************************************************/
static void usage(char const *argv0)
{
    fprintf(stderr, "usage: %s <your-test-program>\n", argv0);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv[0]);
    }
    else
    {
#ifdef SAMPLING
        profiler_sampling_run(argc, argv);
#else
        profiler_events_run(argc, argv);
#endif /* SAMPLING */
    }

    return 0;
}
