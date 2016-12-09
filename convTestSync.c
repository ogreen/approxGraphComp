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

#define NUM_IMPL 4
#define NUM_BIN 20
#define BIN_SPACE .2

#define FISV 0
#define TMSV 1
#define SSSV 2
#define SSHSV 3


int main (const int argc, char *argv[])
{




    int Hist[NUM_IMPL][NUM_BIN];

    for (int i = 0; i < NUM_IMPL; ++i)
    {
        for (int j = 0; j < NUM_BIN; ++j)
        {
            Hist[i][j] = 0;
        }   /* code */
    }
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


    int max_iter = 1000;
    // calling baseline algorithm for checking correctness
    lp_state_t lp_state_bl;
    alloc_lp_state(&graph, &lp_state_bl); // allocate space
    init_lp_state(&graph, &lp_state_bl); // initialize state
    // FFSVAlg_Async(&lp_state_bl,  &graph, &statBL);

    lp_state_bl = FFSVAlg_Sync( &graph, lp_state_bl, &statBL, max_iter);

    max_iter = 20 * statBL.numIteration;

    // Seed random number generator
    int num_trial = 0;
    int seed;
    if (getenv("NUM_TRIAL") != NULL)
    {
        num_trial = (int) atoi(getenv("NUM_TRIAL"));
        // srand(seed);
    }
    else
    {
        printf("Please set the environment variable NUM_TRIAL\n");
        exit(0);
    }

    int norm_prob;
    if (getenv("NORM_PROB") != NULL)
    {
        norm_prob = (int) atoi(getenv("NORM_PROB"));
        // srand(seed);
    }
    else
    {
        printf("Please set the environment variable NORM_PROB\n");
        exit(0);
    }

    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);

    printf("%s Sync %d %d %d\n", GraphName, norm_prob, num_trial, statBL.numIteration);

    lp_state_t lp_state_ssa;
    // int ssf=1000;
    alloc_lp_state(&graph, &lp_state_ssa);
    int ssf = 50;

    
    printf("FISV " );

    for (int i = 0; i < num_trial; ++i)
    {
        /* code */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);
        InitStat(&statSS);

        FISVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );

        int flag = 0;

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                Hist[FISV][NUM_BIN - 1]++;
                flag = 1;
                break;
            }
            // assert(lp_state_ssa.CC[i] == lp_state_bl.CC[i]);
        }
        if (flag == 1)
        {
            continue;
        }
        double overhead = (double) (statSS.numIteration - statBL.numIteration) / ((double) statBL.numIteration );
        printf(", %.2f", overhead );
        int bin_id = (int) (overhead / BIN_SPACE);
        if (bin_id < 0) bin_id = 0;
        if (bin_id > NUM_BIN - 1)
        {
            /* code */
            bin_id = NUM_BIN - 1;
        }

        Hist[FISV][bin_id]++;

    }
    printf("\n");


    
    printf("SSSV " );
    for (int i = 0; i < num_trial; ++i)
    {
        /* code */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);
        InitStat(&statSS);

        SSSVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        int flag = 0;

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                Hist[SSSV][NUM_BIN - 1]++;
                flag = 1;
                break;
            }
            // assert(lp_state_ssa.CC[i] == lp_state_bl.CC[i]);
        }
        if (flag == 1)
        {
            continue;
        }
        double overhead = (double) (statSS.numIteration - statBL.numIteration) / ((double) statBL.numIteration );

        printf(", %.2f", overhead );
        int bin_id = (int) (overhead / BIN_SPACE);
        if (bin_id > NUM_BIN - 1)
        {
            /* code */
            bin_id = NUM_BIN - 1;
        }

        Hist[SSSV][bin_id]++;

    }
    printf("\n");

    printf("SSHSV " );
    for (int i = 0; i < num_trial; ++i)
    {
        /* code */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);
        InitStat(&statSS);

        SSHSVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );

        int flag = 0;

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                Hist[SSHSV][NUM_BIN - 1]++;
                flag = 1;
                break;
            }
            // assert(lp_state_ssa.CC[i] == lp_state_bl.CC[i]);
        }
        if (flag == 1)
        {
            continue;
        }
        double overhead = (double) (statSS.numIteration - statBL.numIteration) / ((double) statBL.numIteration );
        printf(", %.2f", overhead );
        int bin_id = (int) (overhead / BIN_SPACE);
        if (bin_id > NUM_BIN - 1)
        {
            /* code */
            bin_id = NUM_BIN - 1;
        }

        Hist[SSHSV][bin_id]++;

    }
    printf("\n");
    printf("TMSV " );
    for (int i = 0; i < num_trial; ++i)
    {
        /* code */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);
        InitStat(&statSS);

        FISVModAlg_Sync( &lp_state_ssa,  &graph, &statSS, max_iter );
        int flag = 0;

        for (int i = 0; i < graph.numVertices; ++i)
        {

            if (lp_state_ssa.CC[i] != lp_state_bl.CC[i])
            {
                // printf("// Error occured at %d: (%d, %d) \n", i, lp_state_ssa.CC[i], lp_state_bl.CC[i] );
                Hist[TMSV][NUM_BIN - 1]++;
                flag = 1;
                break;
            }
            // assert(lp_state_ssa.CC[i] == lp_state_bl.CC[i]);
        }
        if (flag == 1)
        {
            continue;
        }
        double overhead = (double) (statSS.numIteration - statBL.numIteration) / ((double) statBL.numIteration );
        printf(", %.2f", overhead );
        int bin_id = (int) (overhead / BIN_SPACE);
        if (bin_id > NUM_BIN - 1)
        {
            /* code */
            bin_id = NUM_BIN - 1;
        }

        Hist[TMSV][bin_id]++;

    }
    printf("\n");

    // printGraph(argv[1], &graph, &lp_state_ssa);

    // for (int i = 0; i < NUM_BIN; ++i)
    // {
    //     printf("%.1f %d %d %d %d\n", BIN_SPACE * i, Hist[FISV][i], Hist[SSSV][i], Hist[SSHSV][i], Hist[TMSV][i] );
    // }

    free_graph(&graph);
    free_lp_state(&lp_state_bl);
    free_lp_state(&lp_state_ssa);
    return 0;
}






