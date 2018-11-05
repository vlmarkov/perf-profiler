CC        := g++
CFLAGS    := -g -Wall -std=c++14
INC       := -I include

SRCDIR    := src
TESTDIR   := test
BUILDDIR  := build

all: profiler tests

profiler: perf-event perf-exception perf-utils perf-profiler-factory perf-profiler-events perf-profiler-sampling
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -o $(BUILDDIR)/$@ $(SRCDIR)/main.cpp \
	$(BUILDDIR)/perf-event.o \
	$(BUILDDIR)/perf-exception.o \
	$(BUILDDIR)/perf-utils.o \
	$(BUILDDIR)/perf-profiler-factory.o \
	$(BUILDDIR)/perf-profiler-events.o \
	$(BUILDDIR)/perf-profiler-sampling.o \
	-lpthread -lboost_system

perf-event:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -c -o $(BUILDDIR)/$@.o $(SRCDIR)/$@.cpp

perf-exception:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -c -o $(BUILDDIR)/$@.o $(SRCDIR)/$@.cpp

perf-utils:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -c -o $(BUILDDIR)/$@.o $(SRCDIR)/$@.cpp

perf-profiler-factory:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -c -o $(BUILDDIR)/$@.o $(SRCDIR)/$@.cpp

perf-profiler-events:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -c -o $(BUILDDIR)/$@.o $(SRCDIR)/$@.cpp

perf-profiler-sampling:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) $(INC) -c -o $(BUILDDIR)/$@.o $(SRCDIR)/$@.cpp

tests:
	@echo " Building $@..."
	@mkdir -p $(BUILDDIR);
	$(CC) $(CFLAGS) -o $(BUILDDIR)/loop $(TESTDIR)/sample/loop.cpp;
	$(CC) $(CFLAGS) -o $(BUILDDIR)/hello-world $(TESTDIR)/sample/hello-world.cpp;

clean:
	@echo " Cleaning...";
	rm -Rf $(BUILDDIR)
