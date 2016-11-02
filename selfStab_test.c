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
        printf("usage: %s GraphFile\n", argv[0]);
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


    // calling baseline algorithm for checking

    lp_state_t lp_state_bl;
    alloc_lp_state(&graph, &lp_state_bl); // allocate space
    init_lp_state(&graph, &lp_state_bl); // initialize state
    FFSVAlg_Async(&lp_state_bl,  &graph, &statBL);

    // calling srand to setting initialize random number
    // srand(time(NULL));
    srand(0);

    lp_state_t lp_state_t1;
    alloc_lp_state(&graph, &lp_state_t1);       // allocate space
    rand_lp_state(&graph, &lp_state_t1);    // starting with random state


    SSstep_Async(&graph, &lp_state_t1);   // initializing self-stabilizing step
    FFSVAlg_Async(&lp_state_t1,  &graph, &statFF); // calling LP



    for (int i = 0; i < graph.numVertices; ++i)
    {

        if (lp_state_bl.CC[i] != lp_state_t1.CC[i])
        {
            printf("Error occured at %d: (%d, %d) \n", i, lp_state_bl.CC[i], lp_state_t1.CC[i] );
        }
    }
    printf("Asynchronous Self-stabilizing LP: Passed Random-Init Test ........\n");


    /*get fault probability*/
    double fProb1, fProb2;
    getFault_prob(&fProb1, &fProb2);
    rand_flip_output(fProb1, &graph, &lp_state_t1);
    SSstep_Async(&graph, &lp_state_t1);   // initializing self-stabilizing step
    FFSVAlg_Async(&lp_state_t1,  &graph, &statFF); // calling LP

    for (int i = 0; i < graph.numVertices; ++i)
    {

        if (lp_state_bl.CC[i] != lp_state_t1.CC[i])
        {
            printf("Error occured at %d: (%d, %d) \n", i, lp_state_bl.CC[i], lp_state_t1.CC[i] );
        }
        assert(lp_state_bl.CC[i] == lp_state_t1.CC[i]);
    }
    printf("Asynchronous Self-stabilizing LP: Passed Random-output flip Test ........\n");


    /*testing the Synchronous version*/
    int max_iter = 1000;
    init_lp_state(&graph, &lp_state_bl); // initialize state
    InitStat(&statFF);
    lp_state_bl = FFSVAlg_Sync( &graph, lp_state_bl, &statFF, max_iter);
    printf("Fault free SyncAlg took %d iteration\n", statFF.numIteration);

    rand_flip_output(fProb1, &graph, &lp_state_bl); // random flipping the output
    
    // lp_state_bl.CC[1] =0;
    InitStat(&statFF);
    lp_state_t lp_state_aux;
    alloc_lp_state(&graph, &lp_state_aux);       // allocate space
    SSstep_Sync(&graph, &lp_state_aux, &lp_state_bl);   // initializing self-stabilizing step
    // SSstep_Async(&graph, &lp_state_bl);
    lp_state_bl = FFSVAlg_Sync( &graph, lp_state_aux, &statFF, max_iter);
    printf("SS output flipped Fault free SyncAlg took %d iteration\n", statFF.numIteration);

    for (int i = 0; i < graph.numVertices; ++i)
    {

        if (lp_state_bl.CC[i] != lp_state_t1.CC[i])
        {
            printf("Error occured at %d: (%d, %d) \n", i, lp_state_bl.CC[i], lp_state_t1.CC[i] );
        }
        assert(lp_state_bl.CC[i] == lp_state_t1.CC[i]);
    }
    printf("Synchronous Self-stabilizing LP: Passed Random-output flip Test ........\n");



    free_graph(&graph);
    return 0;
}






