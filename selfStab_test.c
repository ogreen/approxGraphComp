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
#include "faultinjection.h"

#define COUNTOF(array) (sizeof(array) / sizeof(array[0]))


/*test-1: Random Init 
Starts with random starting state; 
runs label propagation algorithm, checks the output with baseline; 
*/
int rand_lp_state( graph_t *graph,
                               lp_state_t *lp_state)
{
    size_t numVertices  = graph->numVertices;

    for (int i = 0; i < numVertices; ++i)
    {
        lp_state->CC[i] = rand();
        lp_state->Ps[i] = rand();
    }
    return 0;
}


/*test-2
Random-flip output test;
Run a correct LP algorithm and randomly flip some of the output at the end.
*/
uint32_t FaultInjectByte(uint32_t value, double prob);
int rand_flip_output( double fprob,             // probability of flipping an entry
    graph_t *graph,
                               lp_state_t *lp_state)
{
    size_t numVertices  = graph->numVertices;

    for (int i = 0; i < numVertices; ++i)
    {
        uint32_t u = lp_state->CC[i];
        lp_state->CC[i] = FaultInjectByte(u, fprob);
        lp_state->Ps[i] = FaultInjectByte(lp_state->Ps[i], fprob);
    }
    return 0;
}


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
    ts_t ts;        //ts_t data structure for tarzen algorithm
    lp_state_t lp_state_t1;
    alloc_lp_state(&graph, &lp_state_t1);       // allocate space
    rand_lp_state(&graph, &lp_state_t1);    // starting with random state
    
    alloc_ts(graph.numVertices, &ts);
    init_ts(graph.numVertices, &ts);
    selfStab_LP(&graph, &lp_state_t1, &ts);   // initializing self-stabilizing step
    FFSVAlg_Async(&lp_state_t1,  &graph, &statFF); // calling LP   



    for (int i = 0; i < graph.numVertices; ++i)
    {
        
        if (lp_state_bl.CC[i] != lp_state_t1.CC[i])
        {
            printf("Error occured at %d: (%d, %d) \n", i, lp_state_bl.CC[i], lp_state_t1.CC[i] );
        }
    }
    printf("Passed Random-Init Test ........\n");


    /*get fault probability*/
    double fProb1, fProb2;
    getFault_prob(&fProb1, &fProb2);
    rand_flip_output(fProb1, &graph, &lp_state_t1);
    selfStab_LP(&graph, &lp_state_t1, &ts);   // initializing self-stabilizing step
    FFSVAlg_Async(&lp_state_t1,  &graph, &statFF); // calling LP   



    for (int i = 0; i < graph.numVertices; ++i)
    {
        
        if (lp_state_bl.CC[i] != lp_state_t1.CC[i])
        {
            printf("Error occured at %d: (%d, %d) \n", i, lp_state_bl.CC[i], lp_state_t1.CC[i] );
        }
        assert(lp_state_bl.CC[i] == lp_state_t1.CC[i]);
    }
    printf("Passed Random-output flip Test ........\n");


    free_ts(&ts);

    
    // PrintCompStat2(GraphName, norm_prob, &statBL, &statFF, &statFT);

    // free_lp_state(&lps_bl);
    // free_lp_state(&lps_ft);
    // free_lp_state(&lps_ff);


    free_graph(&graph);
    return 0;
}






