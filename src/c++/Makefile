all: main

main: perf_event perf_profiler perf_profiler_events perf_profiler_sampling
	g++ -g -Wall -o main \
	main.cpp \
	perf_event.o \
	perf_profiler.o \
	perf_profiler_events.o \
	perf_profiler_sampling.o -std=c++14

perf_event:
	g++ -g -Wall -c -o perf_event.o perf_event.cpp -std=c++14

perf_profiler:
	g++ -g -Wall -c -o perf_profiler.o perf_profiler.cpp -std=c++14

perf_profiler_events:
	g++ -g -Wall -c -o perf_profiler_events.o perf_profiler_events.cpp -std=c++14

perf_profiler_sampling:
	g++ -g -Wall -c -o perf_profiler_sampling.o perf_profiler_sampling.cpp -std=c++14

clean:
	rm -rf main *.o
