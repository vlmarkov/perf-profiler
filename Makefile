BUILD_DIR=build
SRC_DIR=src
EXAMPLE_DIR=examples

all: all-examples c-perf-profiler c++-perf-profiler

all-examples: loop-example hello-world-example

loop-example: $(EXAMPLE_DIR)/loop.c
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -O0 -o $(BUILD_DIR)/loop \
	$(EXAMPLE_DIR)/loop.c -std=gnu99

hello-world-example: $(EXAMPLE_DIR)/hello-world.c
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -o $(BUILD_DIR)/hello-world \
	$(EXAMPLE_DIR)/hello-world.c -std=gnu99


c-perf-profiler: c-perf-lib c-perf-event c-perf-sampling

c-perf-event: c-perf-lib
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -o $(BUILD_DIR)/c-perf-event \
	$(SRC_DIR)/c/main_events.c $(BUILD_DIR)/lib_perf.o -std=gnu99

c-perf-sampling: c-perf-lib
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -o $(BUILD_DIR)/c-perf-sampling \
	$(SRC_DIR)/c/main_sampling.c $(BUILD_DIR)/lib_perf.o -std=gnu99

c-perf-lib: $(SRC_DIR)/c/perf_event.c
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -c -o $(BUILD_DIR)/lib_perf.o \
	$(SRC_DIR)/c/perf_event.c -std=gnu99


c++-perf-profiler: c++_perf_event c++_perf_profiler c++_perf_profiler_events c++_perf_profiler_sampling
	mkdir -p $(BUILD_DIR)
	g++ -g -Wall -o $(BUILD_DIR)/c++-perf-profiler \
	$(SRC_DIR)/c++/main.cpp \
	$(BUILD_DIR)/c++_perf_event.o \
	$(BUILD_DIR)/c++_perf_profiler.o \
	$(BUILD_DIR)/c++_perf_profiler_events.o \
	$(BUILD_DIR)/c++_perf_profiler_sampling.o -std=c++14

c++_perf_event:
	mkdir -p $(BUILD_DIR)
	g++ -g -Wall -c -o $(BUILD_DIR)/c++_perf_event.o \
	$(SRC_DIR)/c++/perf_event.cpp -std=c++14

c++_perf_profiler:
	mkdir -p $(BUILD_DIR)
	g++ -g -Wall -c -o $(BUILD_DIR)/c++_perf_profiler.o \
	$(SRC_DIR)/c++/perf_profiler.cpp -std=c++14

c++_perf_profiler_events:
	mkdir -p $(BUILD_DIR)
	g++ -g -Wall -c -o $(BUILD_DIR)/c++_perf_profiler_events.o \
	$(SRC_DIR)/c++/perf_profiler_events.cpp -std=c++14

c++_perf_profiler_sampling:
	mkdir -p $(BUILD_DIR)
	g++ -g -Wall -c -o $(BUILD_DIR)/c++_perf_profiler_sampling.o \
	$(SRC_DIR)/c++/perf_profiler_sampling.cpp -std=c++14


clean:
	rm -Rf $(BUILD_DIR)

