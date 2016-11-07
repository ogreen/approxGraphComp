/* file consists of protocols for testing self-stabilizing algorithms */

#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include "sv.h"
#include "stat.h"
#include "graph.h"
#include "sssv.h"
#include "faultInjection.h"

#define COUNTOF(array) (sizeof(array) / sizeof(array[0]))


int main (const int argc, char *argv[])
{



    uint32_t nv, ne, naction;
    uint32_t* off;
    uint32_t* ind;
    if (argc < 2)
    {
        printf("// usage: %s GraphFile\n", argv[0]);
        exit(1);

    }
    const char dlm[2] = ".";

    char *GraphName = ExtractGraphName(argv[1]);

    /*initializing the graph*/
    graph_t graph;
    readGraphDIMACS(argv[1], &graph);

    /*starting the stat*/
    stat_t statBL, statSS;
    InitStat(&statBL);
    InitStat(&statSS);
    


    // calling baseline algorithm for checking correctness
    lp_state_t lp_state_bl;
    alloc_lp_state(&graph, &lp_state_bl); // allocate space
    init_lp_state(&graph, &lp_state_bl); // initialize state
    // FFSVAlg_Async(&lp_state_bl,  &graph, &statBL);
    int max_iter = 1000;
    lp_state_bl = FFSVAlg_Sync( &graph, lp_state_bl, &statBL, max_iter);

    // Seed random number generator
    int seed;
    if (getenv("RSEED") != NULL)
    {
        seed = (double) atoi(getenv("RSEED"));
        srand(seed);
    }
    else
    {
        printf("Please set the environment variable RSEED\n");
        exit(0);
    }

    lp_state_t lp_state_ssa;
    // int ssf=1000;
    alloc_lp_state(&graph, &lp_state_ssa); 
    init_lp_state(&graph, &lp_state_ssa); 
    InitStat(&statSS);
    SSSVAlg_Sync( &lp_state_ssa,  &graph, &statSS, max_iter );

    for (int i = 0; i < graph.numVertices; ++i)
    {

        if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
        {
            printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
        }
        assert(lp_state_ssa.CC[i] == lp_state_bl.CC[i]);
    }

    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);

    printf("%s %e %e %d %d\n",GraphName, fProb1,fProb2, statBL.numIteration,statSS.numIteration );
    free_graph(&graph);
    free_lp_state(&lp_state_bl);
    free_lp_state(&lp_state_ssa);
    return 0;
}






