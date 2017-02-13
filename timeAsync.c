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

    int norm_prob;
    if (getenv("NORM_PROB") != NULL)
    {
        norm_prob = (double) atof(getenv("NORM_PROB"));

    }
    else
    {
        norm_prob = 0;
    }

    /*initializing the graph*/
    graph_t graph;
    readGraphDIMACS(argv[1], &graph);

    /*starting the stat*/

    stat_t statBL, statFF, statFT;
    InitStat(&statBL);
    InitStat(&statFF);
    InitStat(&statFT);


    // calling baseline synchronous algorithm for checking
    int max_iter = 1000;

    lp_state_t lp_state_bl;
    alloc_lp_state(&graph, &lp_state_bl); // allocate space
    init_lp_state(&graph, &lp_state_bl); // initialize state
    
    FFSVAlg_Sync(&graph, lp_state_bl, &statBL, max_iter);
    
    FFSVAlg_Sync(&graph, lp_state_bl, &statBL, max_iter);
    // printf("//\n");

    // calling srand to setting initialize random number
    // srand(time(NULL));
    srand(0);

    lp_state_t lp_state_t1;
    alloc_lp_state(&graph, &lp_state_t1);       // allocate space
    rand_lp_state(&graph, &lp_state_t1);    // starting with random state

    int NUM_TRIALS=10;
        double t_ss=0;
        double t_sv=0;

    for (int i = 0; i < NUM_TRIALS; ++i)
    {
        /* code */
        tic();
        SSstep_Async(&graph, &lp_state_t1);   // initializing self-stabilizing step
        t_ss += toc();

        tic();
        FFSVAlg_Async(&lp_state_t1,  &graph, &statFF); // calling LP
        t_sv += toc();

    }
    
    printf("%s %f %f\n", GraphName, t_ss/NUM_TRIALS, t_sv/NUM_TRIALS );

    return 0;
}

