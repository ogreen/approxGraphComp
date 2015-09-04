.PHONY:	all clean 
all: sv 

DEFINES = 
ifeq ($(INTEL_HASWELL_COUNTERS),1)
	DEFINES += -DHAVE_INTEL_HASWELL_COUNTERS
endif
ifeq ($(INTEL_IVYBRIDGE_COUNTERS),1)
	DEFINES += -DHAVE_INTEL_IVYBRIDGE_COUNTERS
endif
ifeq ($(INTEL_SILVERMONT_COUNTERS),1)
	DEFINES += -DHAVE_INTEL_SILVERMONT_COUNTERS
endif
ifeq ($(AMD_FAMILY15_COUNTERS),1)
	DEFINES += -DHAVE_AMD_FAMILY15_COUNTERS
endif

sv: main.c timer.c sv.c Makefile
	$(CC) -g -O3 -std=gnu99 $(CFLAGS) $(DEFINES) -Wno-unused-result -DBENCHMARK_SV -o $@ main.c timer.c sv.c  $(LDFLAGS) -lrt

 
clean:
	-rm -f bfs
	-rm -f sv
	-rm -f cct
	-rm -f *.o
