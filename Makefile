BUILD_DIR=build
SRC_DIR=src
EXAMPLE_DIR=examples

all: all-examples perf-sampling perf-events

all-examples: loop-example hello-world-example

loop-example: $(EXAMPLE_DIR)/loop.c
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -O0 -o $(BUILD_DIR)/loop $(EXAMPLE_DIR)/loop.c -std=gnu99

hello-world-example: $(EXAMPLE_DIR)/hello-world.c
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -o $(BUILD_DIR)/hello-world $(EXAMPLE_DIR)/hello-world.c -std=gnu99

perf-events: perf-lib
	mkdir -p $(SRC_DIR)
	gcc -g -Wall -o $(BUILD_DIR)/perf-events $(SRC_DIR)/main_events.c $(BUILD_DIR)/lib_perf.o -std=gnu99

perf-sampling: perf-lib
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -o $(BUILD_DIR)/perf-sampling $(SRC_DIR)/main_sampling.c $(BUILD_DIR)/lib_perf.o -std=gnu99

perf-lib: $(SRC_DIR)/perf_event.c
	mkdir -p $(BUILD_DIR)
	gcc -g -Wall -c -o $(BUILD_DIR)/lib_perf.o $(SRC_DIR)/perf_event.c -std=gnu99

clean:
	rm -Rf $(BUILD_DIR)

