/* file consist of main function that report number
of failures of label propagation algorithm for a given
graph and fault rate: For Sync Algorithm */

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
#include "ft_sv.h"
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

    int max_iter = 100;
    int ssf = 50;
    if (getenv("MAX_ITER") != NULL)
    {
        max_iter = (double) atoi(getenv("MAX_ITER"));

    }
    else
    {
        printf("Please set the environment variable MAX_ITER\n");
        exit(0);
    }

    // calling baseline algorithm for checking correctness
    lp_state_t lp_state_bl;
    alloc_lp_state(&graph, &lp_state_bl); // allocate space
    init_lp_state(&graph, &lp_state_bl); // initialize state
    // FFSVAlg_Async(&lp_state_bl,  &graph, &statBL);

    lp_state_bl = FFSVAlg_Sync( &graph, lp_state_bl, &statBL, max_iter);

    // Seed random number generator
    int seed;
    int num_trials;
    if (getenv("NUM_TRIAL") != NULL)
    {
        num_trials = (double) atoi(getenv("NUM_TRIAL"));

    }
    else
    {
        printf("Please set the environment variable NUM_TRIAL\n");
        exit(0);
    }



    lp_state_t lp_state_ssa;
    printGraph(argv[1], &graph, &lp_state_ssa);

    // int ssf=1000;
    alloc_lp_state(&graph, &lp_state_ssa);

    int num_failures = 0;

    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);

        InitStat(&statSS);

        FISVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        // SSSVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        // printf("%s\n", );

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                num_failures++;
                break;
            }

        }

    }

    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);

    printf("%s %e %e %d %d ", GraphName, fProb1, fProb2, num_trials, num_failures );

    num_failures = 0;

    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);

        InitStat(&statSS);

        // FISVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        SSSVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        // printf("%s\n", );

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                num_failures++;
                break;
            }

        }

    }

    printf(" %d ", num_failures );

    num_failures = 0;

    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);

        InitStat(&statSS);

        // FISVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        SSHSVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        // printf("%s\n", );

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                num_failures++;
                break;
            }

        }

    }

    printf(" %d ", num_failures );

    num_failures = 0;

    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);

        InitStat(&statSS);

        // FISVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );

        FISVModAlg_Sync( &lp_state_ssa,  &graph, &statSS, max_iter );
        // printf("%s\n", );

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                num_failures++;
                break;
            }

        }

    }

    printf(" %d ", num_failures );

    printGraph(argv[1], &graph, &lp_state_ssa);

    free_graph(&graph);
    free_lp_state(&lp_state_bl);
    free_lp_state(&lp_state_ssa);
    return 0;
}






