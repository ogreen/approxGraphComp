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
#include "sv.h"
#include "stat.h"
#include "graph.h"

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

    int max_iter = 1000;

    lp_state_t lps_bl = FFWoSVAlg_Sync( &graph, &statBL);

    lp_state_t lps_ft = SCSVAlg_Sync( &graph, &statFT, max_iter);

    lp_state_t lps_ff; 
    alloc_lp_state(&graph, &lps_ff);    // allocate space
    init_lp_state(&graph, &lps_ff);     // initialize state
    lps_ff = FFSVAlg_Sync( &graph, lps_ff, &statFF, max_iter);

    // find number of components in the graphs
    printf("NUmber of Components in %s = %d \n",GraphName, numComponent(&graph, &lps_ff));

    for (int i = 0; i < graph.numVertices; ++i)
    {
        
        if (lps_ft.CC[i] != lps_ff.CC[i])
        {
            printf("Error occured at %d: (%d, %d) \n", i, lps_ft.CC[i], lps_ff.CC[i] );
        }
        

    }

    for (int i = 0; i < graph.numVertices; ++i)
    {
        /* code */
        assert(lps_ff.CC[i] == lps_bl.CC[i]);

    }

    // statFT.numIteration = 50;

    // PrintCompStat2(GraphName, norm_prob, &statBL, &statFF, &statFT);

    free_lp_state(&lps_bl);
    free_lp_state(&lps_ft);
    free_lp_state(&lps_ff);


    free_graph(&graph);
    return 0;
}






