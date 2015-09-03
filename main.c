#include "main.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define PERF_TYPE_TIME PERF_TYPE_MAX

struct PerformanceCounter {
    const char* name;
    uint32_t type;
    uint32_t subtype;
    bool supported;
};

void CheckPerformanceCounters(struct PerformanceCounter performanceCounters[], size_t performanceCountersCount);


 typedef struct {
  int32_t iterationSwap;  
  float   toleranceGradient;
  float	  toleranceSteadyState;
} svControlParams;
  

#define LINE_SIZE 10000


static int perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
	return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

void CheckPerformanceCounters(struct PerformanceCounter performanceCounters[], size_t performanceCountersCount) {
    for (size_t performanceCounterIndex = 0; performanceCounterIndex < performanceCountersCount; performanceCounterIndex++) {
        if (performanceCounters[performanceCounterIndex].type == PERF_TYPE_TIME) {
            performanceCounters[performanceCounterIndex].supported = true;
            continue;
        }
        
		struct perf_event_attr perf_counter;
        memset(&perf_counter, 0, sizeof(struct perf_event_attr));
        perf_counter.type = performanceCounters[performanceCounterIndex].type;
        perf_counter.size = sizeof(struct perf_event_attr);
        perf_counter.config = performanceCounters[performanceCounterIndex].subtype;
        perf_counter.disabled = 1;
        perf_counter.exclude_kernel = 1;
        perf_counter.exclude_hv = 1;

        performanceCounters[performanceCounterIndex].supported = true;
        int perf_counter_fd = perf_event_open(&perf_counter, 0, -1, -1, 0);
        if (perf_counter_fd == -1) {
            performanceCounters[performanceCounterIndex].supported = false;
            continue;
        }

        if(ioctl(perf_counter_fd, PERF_EVENT_IOC_RESET, 0) != 0) {
            performanceCounters[performanceCounterIndex].supported = false;
        } else {
            if (ioctl(perf_counter_fd, PERF_EVENT_IOC_ENABLE, 0) != 0) {
                performanceCounters[performanceCounterIndex].supported = false;
            } else {
                if (ioctl(perf_counter_fd, PERF_EVENT_IOC_DISABLE, 0) != 0) {
                    performanceCounters[performanceCounterIndex].supported = false;
                } else {
                    uint64_t dummy;
                    if (read(perf_counter_fd, &dummy, sizeof(uint64_t)) != sizeof(uint64_t)) {
                        performanceCounters[performanceCounterIndex].supported = false;
                    }
                }
            }
            if (close(perf_counter_fd) != 0) {
                performanceCounters[performanceCounterIndex].supported = false;
            }
        }
    }
}

void PrintHeader(const char* precolumns[], const struct PerformanceCounter performanceCounters[], size_t performanceCountersCount, const char* postcolumns[]) {
    bool firstColumn = true;
    while (*precolumns != NULL) {
        if (firstColumn) {
            printf("%s", *precolumns);
            firstColumn = false;
        } else {
            printf("\t%s", *precolumns);
        }
        precolumns++;
    }
    for (size_t performanceCounterIndex = 0; performanceCounterIndex < performanceCountersCount; performanceCounterIndex++) {
        if (!performanceCounters[performanceCounterIndex].supported)
            continue;
        printf("\t%s", performanceCounters[performanceCounterIndex].name);
    }
    while (*postcolumns != NULL) {
        printf("\t%s", *postcolumns);
        postcolumns++;
    }
    printf("\n");
}


#define COUNTOF(array) (sizeof(array) / sizeof(array[0]))

void readGraphDIMACS(char* filePath, uint32_t** prmoff, uint32_t** prmind, uint32_t* prmnv, uint32_t* prmne){
	FILE *fp = fopen (filePath, "r");
	int32_t nv,ne;

	char* line=NULL;
	
	// Read data from file
	int32_t temp,lineRead;
	size_t bytesRead=0;
	getline (&line, &bytesRead, fp);	
	
//	fgets (line, bytesRead, fp);
	sscanf (line, "%d %d", &nv, &ne);
//	printf ( "%ld %ld\n", nv, ne);		
		
	free(line);
	int32_t * off = (int32_t *) malloc ((nv + 2) * sizeof (int32_t));
	int32_t * ind = (int32_t *) malloc ((ne * 2) * sizeof (int32_t));
	off[0] = 0;
	off[1] = 0;
	int32_t counter = 0;
	int32_t u;
	line=NULL;
	bytesRead=0;

//	  for (u = 1; fgets (line, &bytesRead, fp); u++)
	for (u = 1; (temp=getline (&line, &bytesRead, fp))!=-1; u++)
	{	
//		printf("%s",line);	,
/*		bytesRead=0;	
		free(line);	
		if (u>10) 
			break;

		continue;
*/		
		uint32_t neigh = 0;
		uint32_t v = 0;
		char *ptr = line;
		int read = 0;
		char tempStr[1000];
		lineRead=0;
		while (lineRead<bytesRead && (read=sscanf (ptr, "%s", tempStr)) > 0)
		{
			v=atoi(tempStr);
			read=strlen(tempStr);
			ptr += read+1;
			lineRead=read+1;
			neigh++;
			ind[counter++] = v;
		}
		off[u + 1] = off[u] + neigh;
		free(line);	  
		bytesRead=0;
	}


	  fclose (fp);


	   nv++;
	   ne*=2;
	*prmnv=nv;
	*prmne=ne;
	*prmind=ind;
	*prmoff=off;
}

int main (const int argc, char *argv[]) {

    uint32_t nv, ne, naction;
    uint32_t* off;
    uint32_t* ind;
	readGraphDIMACS(argv[1], &off, &ind, &nv, &ne);
	
    struct PerformanceCounter perfCounters[] = {
        { "Time", PERF_TYPE_TIME },
        { "Cycles", PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES},
        { "Instructions", PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS},
    #if defined(HAVE_INTEL_HASWELL_COUNTERS) || defined(HAVE_INTEL_IVYBRIDGE_COUNTERS)
        { "Loads.Retired", PERF_TYPE_RAW, 0x81D0 }, // D0H 01H MEM_UOPS_RETIRED.LOADS
        { "Stores.Retired", PERF_TYPE_RAW, 0x82D0 }, // D0H 01H MEM_UOPS_RETIRED.STORES
        { "Stall.RS", PERF_TYPE_RAW, 0x04A2 }, // A2H 04H RESOURCE_STALLS.RS Cycles stalled due to no eligible RS entry available. 
        { "Stall.SB", PERF_TYPE_RAW, 0x08A2 }, // A2H 08H RESOURCE_STALLS.SB Cycles stalled due to no store buffers available (not including draining form sync).
        { "Stall.ROB", PERF_TYPE_RAW, 0x10A2 }, // A2H 10H RESOURCE_STALLS.ROB
    #endif
    #if defined(HAVE_INTEL_SILVERMONT_COUNTERS)
        { "Stall.ROB", PERF_TYPE_RAW, 0x01CA }, // CAH 01H NO_ALLOC_CYCLES.ROB_FULL Counts the number of cycles when no uops are allocated and the ROB is full (less than 2 entries available)
        { "Stall.RAT", PERF_TYPE_RAW, 0x20CA }, // CAH 01H NO_ALLOC_CYCLES.RAT_STALL Counts the number of cycles when no uops are allocated and a RATstall is asserted. 
        { "Stall.MEC", PERF_TYPE_RAW, 0x01CB }, // CBH 01H RS_FULL_STALL.MEC MEC RS full This event countsthe number of cycles the allocation pipe line stalled due to the RS for the MEC cluster is full
        { "Stall.AnyRS", PERF_TYPE_RAW, 0x1FCB }, // CBH 1FH RS_FULL_STALL.ALL Any RS full This event countsthe number of cycles that the allocation pipe line stalled due to any one of the RS is full
        { "Loads.RehabQ", PERF_TYPE_RAW, 0x4003 }, // 03H 40H REHABQ.ANY_LD Any reissued load uops This event counts the number of loaduops reissued from Rehabq
        { "Stores.RehabQ", PERF_TYPE_RAW, 0x8003 }, // 03H 80H REHABQ.ANY_ST Any reissued store uops This event counts the number of store uops reissued from Rehabq
        { "Loads.Retired", PERF_TYPE_RAW, 0x4004 }, // 04H 40H MEM_UOPS_RETIRED.ALL_LOADS All Loads  This event counts the number of load ops retired 
        { "Stores.Retired", PERF_TYPE_RAW, 0x8004 }, // 04H 80H MEM_UOP_RETIRED.ALL_STORES All Stores  This event counts the number of store ops retired
    #endif
    #if defined(HAVE_AMD_FAMILY15_COUNTERS)
        { "Stall.SB", PERF_TYPE_RAW, 0x0223 }, // The number of cycles that the store buffer is full.
        { "Stall.LB", PERF_TYPE_RAW, 0x0123 }, // The number of cycles that the load buffer is full.
        { "Loads.Dispatched", PERF_TYPE_RAW, 0x0129 },
        { "Stores.Dispatched", PERF_TYPE_RAW, 0x0229 },
        { "Stall.LDQ", PERF_TYPE_RAW, 0x01D8 }, // Dispatch Stall for LDQ Full
    #endif
        { "Cache references", PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES},
        { "Cache misses", PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES},
        { "Branches", PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS},
        { "Mispredictions", PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES}
    };
    
    CheckPerformanceCounters(perfCounters, COUNTOF(perfCounters));


#if defined(BENCHMARK_SV)
	  const char* precolumns[] = {
		  "Algorithm",
		  "Implementation",
		  "Itetarion",
		  NULL
	  };
	  const char* postcolumns[] = {
		  "Vertices",
		  "Edges",
		  NULL
	  };



	  PrintHeader(precolumns, perfCounters, COUNTOF(perfCounters), postcolumns);
//	  Benchmark_ConnectedComponents_SV("SV", "Branch-based", perfCounters, COUNTOF(perfCounters), sv_function,iterBB, nv, ne, off, ind);
//	  Benchmark_ConnectedComponents_SV("SV", "Branch-avoiding", perfCounters, COUNTOF(perfCounters), sv_function,iterBA, nv, ne, off, ind);
	 
//	  ConnectedComponentsSVHybridIterationSelector ("SV", "Branch-Hybrid", perfCounters, COUNTOF(perfCounters), sv_function,iterHybrid,svCP, nv, ne, off, ind);
//	  Benchmark_ConnectedComponents_SV("SV", "Branch-Hybrid", perfCounters, COUNTOF(perfCounters), sv_function,iterHybrid, nv, ne, off, ind);
//	  Benchmark_ConnectedComponents_SV("SV", "Branch-avoiding", perfCounters, COUNTOF(perfCounters), ConnectedComponents_SV_Branchless_PeachPy, nv, ne, off, ind);
//	  Benchmark_ConnectedComponents_SV("SV", "Hybrid", perfCounters, COUNTOF(perfCounters), ConnectedComponents_SV_Branchless_PeachPy, nv, ne, off, ind);
	  #ifdef __SSE4_1__
//	  Benchmark_ConnectedComponents_SV("SV", "SSE4.1", perfCounters, COUNTOF(perfCounters), ConnectedComponents_SV_Branchless_SSE4_1, nv, ne, off, ind);
	  #endif
	  #ifdef __MIC__
//	  Benchmark_ConnectedComponents_SV("SV", "MIC", perfCounters, COUNTOF(perfCounters), ConnectedComponents_SV_Branchless_MIC, nv, ne, off, ind);
	  #endif

#endif

 	free(off);
	free(ind);
	return 0;
}


#if defined(BENCHMARK_SV)
	void Benchmark_ConnectedComponents_SV(const char* algorithm_name, const char* implementation_name, const struct PerformanceCounter performanceCounters[], size_t performanceCounterCount, ConnectedComponents_SV_Function sv_function, size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind){
		struct perf_event_attr perf_counter;

		uint32_t* components_map = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
        uint64_t* perf_events = (uint64_t*)malloc(numVertices * sizeof(uint64_t));
        uint32_t* vertices = (uint32_t*)malloc(numVertices * sizeof(uint32_t));

        uint32_t iterationCount = 0;
        for (size_t performanceCounterIndex = 0; performanceCounterIndex < performanceCounterCount; performanceCounterIndex++) {
            if (!performanceCounters[performanceCounterIndex].supported)
                continue;
            int perf_counter_fd = -1;
            if (performanceCounters[performanceCounterIndex].type != PERF_TYPE_TIME) {
                memset(&perf_counter, 0, sizeof(struct perf_event_attr));
                perf_counter.type = performanceCounters[performanceCounterIndex].type;
                perf_counter.size = sizeof(struct perf_event_attr);
                perf_counter.config = performanceCounters[performanceCounterIndex].subtype;
                perf_counter.disabled = 1;
                perf_counter.exclude_kernel = 1;
                perf_counter.exclude_hv = 1;

                perf_counter_fd = perf_event_open(&perf_counter, 0, -1, -1, 0);
                if (perf_counter_fd == -1) {
                    fprintf(stderr, "Error opening counter %s\n", performanceCounters[performanceCounterIndex].name);
                    exit(EXIT_FAILURE);
                }
            }

            /* Initialize level array */
            for (size_t i = 0; i < numVertices; i++) {
                components_map[i] = i;
            }

            bool changed;
            size_t iteration = 0;
            do {
                struct timespec startTime;
                if (performanceCounters[performanceCounterIndex].type == PERF_TYPE_TIME) {
                    assert(clock_gettime(CLOCK_MONOTONIC, &startTime) == 0);
                } else {
                    assert(ioctl(perf_counter_fd, PERF_EVENT_IOC_RESET, 0) == 0);
                    assert(ioctl(perf_counter_fd, PERF_EVENT_IOC_ENABLE, 0) == 0);
                }

                //printf("@@ %d @@ ",algPerIteration[iteration]);
                changed = (sv_function)(numVertices, components_map, off, ind);

                if (performanceCounters[performanceCounterIndex].type == PERF_TYPE_TIME) {
                    struct timespec endTime;
                    assert(clock_gettime(CLOCK_MONOTONIC, &endTime) == 0);
                    perf_events[iterationCount * performanceCounterIndex + iteration] =
                        (1000000000ll * endTime.tv_sec + endTime.tv_nsec) - 
                        (1000000000ll * startTime.tv_sec + startTime.tv_nsec);
                } else {
                    assert(ioctl(perf_counter_fd, PERF_EVENT_IOC_DISABLE, 0) == 0);
                    assert(read(perf_counter_fd, &perf_events[iterationCount * performanceCounterIndex + iteration], sizeof(uint64_t)) == sizeof(uint64_t));
                }
                iteration += 1;
            } while (changed);
            if (iterationCount == 0) {
                iterationCount = iteration;
                perf_events = realloc(perf_events, numVertices * sizeof(uint64_t) * iterationCount);
            }
            close(perf_counter_fd);
        }
        for (uint32_t iteration = 0; iteration < iterationCount; iteration++) {
            printf("%s\t%s\t%"PRIu32, algorithm_name, implementation_name, iteration);
            for (size_t performanceCounterIndex = 0; performanceCounterIndex < performanceCounterCount; performanceCounterIndex++) {
                if (!performanceCounters[performanceCounterIndex].supported)
                    continue;
                printf("\t%"PRIu64, perf_events[iterationCount * performanceCounterIndex + iteration]);
            }
            printf("\t%zu\t%zu\n", numVertices, numEdges);
        }
        free(components_map);
		free(perf_events);
		free(vertices);
	}


 

#endif
