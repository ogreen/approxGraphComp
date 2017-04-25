/* file consist of main function that report number
of failures of label propagation algorithm for a given
graph and a calculated fault rate for the graph.

The fault rate is calculated where TMR fails at specified
rate.

We sample TMR 20 times to get the fault rate.

we perform a binary search on fault rates from

2^-{n} where n is between 5 and 20 (can be set by environment variable)
tmrFr is desired failure rate for TMR algo:

let F(n) = failure rate of tmr for the input graph
perform binary search to get an n such that
0<F(n)<tmrFr \leq F(n+1)

F(n) is calculated by performing a sample of 20 times for the n and the given fault rate


 and fault rate: For Async Algorithm */

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




double tmrFr_n(int n,
               int num_trials, int max_iter,
               graph_t *graph, lp_state_t* lp_state_bl,
               lp_state_t* lp_state_ssa, stat_t* statSS )
{
    
    char ptstr[20];
    sprintf(ptstr,"%d",n );
    setenv("NORM_PROB", ptstr, 1);
    int num_failures = 0;
    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i + 94385);
        init_lp_state(graph, lp_state_ssa);

        InitStat(statSS);



        FISVModAlg_Async( lp_state_ssa,  graph, statSS, max_iter);

        for (int i = 0; i < graph->numVertices; ++i)
        {

            if (lp_state_ssa->CC[i] != lp_state_bl->CC[i])
            {
                num_failures++;
                break;
            }

        }

    }

    double fr = (1.0 * num_failures) / num_trials;

    return fr;


}


int getNrmFr(double tmrTargetFr, int max_iter,
               graph_t *graph, lp_state_t* lp_state_bl,
               lp_state_t* lp_state_ssa, stat_t* statSS )
{
    double frArr[32] ={0};
    int leftn = 7;
    int rightn = 23;
    int steps = 0;
    double tmrFr;
    int curn;
    int num_trials = 20;
    // int max_iter = 

    while (rightn > leftn + 1)
    {
        steps++;
        curn = (rightn + leftn) / 2;
        //printf("step=%d, leftn=%d, curn=%d, rightn=%d \n", steps, leftn, curn, rightn);
        // tmrFr = getTmrFr(curn);
        tmrFr = tmrFr_n(curn, num_trials, max_iter,
               graph, lp_state_bl,
               lp_state_ssa, statSS );
        frArr[curn] = tmrFr;
        // printf("step=%d, leftn=%d, curn=%d, rightn=%d F(curn)=%.3f\n", steps, leftn, curn, rightn,tmrFr);
        if (tmrFr > tmrTargetFr)
        {
            leftn = curn;
        }
        else 
        {
            rightn = curn;
        }

    }

    double e1 = fabs(tmrTargetFr-frArr[curn-1]);
    double e2 = fabs(tmrTargetFr-frArr[curn]);
    double e3 = fabs(tmrTargetFr-frArr[curn+1]);

    int nrmFr=curn;
    if (e1<e2 && e1<e3)
    {
        /* code */
        tmrFr= frArr[curn-1];
        nrmFr = curn-1;
    }

    if (e2<e1 && e2<e3)
    {
        /* code */
        tmrFr= frArr[curn];
        nrmFr = curn;
    }

    if (e3<e1 && e3<e1)
    {
        /* code */
        tmrFr= frArr[curn+1];
        nrmFr = curn+1;
    }

    printf("%.3f  %.3f  %d  %d ",
           tmrTargetFr, tmrFr, nrmFr, steps);


    return nrmFr;
}




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
    
    // setting print graph to 0
    setenv("PRINT_GRAPH", "0", 1);


    FFSVAlg_Async( &lp_state_bl, &graph, &statBL);  // task 0

    double tmrTargetFr;
    if (getenv("TMR_FR") != NULL)
    {
        tmrTargetFr = (double) atof(getenv("TMR_FR"));

    }
    else
    {
        printf("Please set the environment variable TMR_FR\n");
        exit(0);
    }

    lp_state_t lp_state_ssa;
    alloc_lp_state(&graph, &lp_state_ssa);

    // get the normalized fault rate
    int nrmFr= getNrmFr( tmrTargetFr, max_iter,
                    &graph,  &lp_state_bl,
                    &lp_state_ssa, &statSS );

    char ptstr[20];
    sprintf(ptstr,"%d",nrmFr );
    setenv("NORM_PROB", ptstr, 1);

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

    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);

    
    printf("%s %e %e %d ", GraphName, fProb1, fProb2, num_trials);

    // return 0; // r=early return for now 





    
    printGraph(argv[1], &graph, &lp_state_ssa);

    // int ssf=1000;


    int num_failures = 0;

    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);

        InitStat(&statSS);

        FISVAlg_Async( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
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


    printf(" %d ", num_failures );

    num_failures = 0;

    for (int i = 0; i < num_trials; ++i)
    {
        /* set random numbr seed for the trial */
        srand(i);
        init_lp_state(&graph, &lp_state_ssa);

        InitStat(&statSS);

        // FISVAlg_Sync( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
        SSSVAlg_Async( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
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
        SSHSVAlg_Async( &lp_state_ssa,  &graph, &statSS, ssf, max_iter );
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

        FISVModAlg_Async( &lp_state_ssa,  &graph, &statSS, max_iter );
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

    

    free_graph(&graph);
    free_lp_state(&lp_state_bl);
    free_lp_state(&lp_state_ssa);
    return 0;
}






