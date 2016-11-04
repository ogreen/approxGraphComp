
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <string.h>

#include "timer.h"
#include "stat.h"
#include "faultInjection.h"
#include "sv.h"
static long long MemAccessCount;


int alloc_lp_state(graph_t *graph, lp_state_t *lp_state)
{
    size_t numVertices  = graph->numVertices;

    lp_state->CC = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    lp_state->Ps = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    lp_state->P = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    lp_state->Cr = (int*)memalign(64, numVertices * sizeof(int));
    return 0;
}

int init_lp_state(graph_t *graph, lp_state_t *lp_state)
{
    size_t numVertices  = graph->numVertices;

    for (int i = 0; i < numVertices; ++i)
    {
        lp_state->CC[i] = i;
        lp_state->Ps[i] = -1;
        lp_state->Cr[i] = 0;
    }
    return 0;
}


int free_lp_state(lp_state_t *lp_state)
{
    free(lp_state->CC);
    free(lp_state->Ps);
    free(lp_state->P);
    free(lp_state->Cr);
}


int printParentTree(char *name, graph_t* graph, lp_state_t *lp_state)
// prints a directed graph in dot format with title as s
{
    if (getenv("PRINT_GRAPH") != NULL)
    {
        int  ind = (int) atoi(getenv("PRINT_GRAPH"));
        if(ind==0) return 0;
    }
    else
    {
        return 0;
    }
    size_t nv = graph->numVertices;
    printf("digraph %s { \n", name);
    printf("rankdir=BT\n");
    uint32_t *CC = lp_state->CC;

    for (uint32_t v = 0; v < nv; v++)
    {
        // printf(" %d %d %d %d\n", v, lp_state->Ps[v], graph->off[v] );
        if (graph->off[v]  + lp_state->Ps[v] < graph->numEdges || lp_state->Ps[v] == -1)
        {
            uint32_t Pv = lp_state->Ps[v] == -1 ? v : graph->ind[graph->off[v]  + lp_state->Ps[v]];
            printf("\"%d|%d\" -> \"%d|%d\";\n", v, CC[v], Pv, CC[Pv] );
        }
    }
    printf("labelloc=\"t\"\n");
    printf("label=\"%s\"\n", name);
    printf("}\n");
}

int printGraph(char *name, graph_t* graph, lp_state_t *lp_state)
{
    // return 0;
    if (getenv("PRINT_GRAPH") != NULL)
    {
        int  ind = (int) atoi(getenv("PRINT_GRAPH"));
        if(ind==0) return 0;
    }
    else
    {
        return 0;
    }
    size_t nv = graph->numVertices;
    uint32_t* off = graph->off;
    uint32_t* ind = graph->ind;

    printf("graph input { \n");
    printf("rankdir=BT\n");

    for (uint32_t v = 0; v < nv; v++)
    {
        uint32_t *vind = &ind[off[v]];
        size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];

            if (v > u)
                printf("%d -- %d;\n", v, u );

        }
        

    }
    printf("labelloc=\"t\"\n");
    printf("label=\"%s\"\n", name);
    printf("}\n");
}

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




// bool FFSVSweep_Async(size_t nv, uint32_t* component_map, uint32_t* off, uint32_t* ind)
bool FFSVSweep_Async(graph_t *graph, lp_state_t *lp_state)
{
    size_t nv = graph->numVertices;
    uint32_t* off = graph->off;
    uint32_t* ind = graph->ind;
    uint32_t* component_map = lp_state->CC;

    bool changed = false;

    for (size_t v = 0; v < nv; v++)
    {
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];
            if (component_map[u] < component_map[v])
            {
                lp_state->Ps[v] = edge;
                component_map[v] = component_map[u];
                changed = true;
            }
        }
    }

    /*shortcutting goes here*/
    return changed;

}



int FFSVAlg_Async( lp_state_t *lp_state,  graph_t *graph,
                   stat_t* stat)
{
    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;

    bool changed;
    size_t iteration = 0;
    do
    {
        changed = FFSVSweep_Async(graph, lp_state);
        iteration += 1;
    }
    while (changed);

    /*updating stats*/
    stat->numIteration = iteration;
    printf("// Number of iteration is %d\n", iteration );

    return 0;
}

/*fault tolerant SV sweep */
int FFWoSVSweep_Sync(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                     uint32_t* off, uint32_t* ind)
{

    int changed = 0;
    // #pragma omp parallel for
    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        MemAccessCount += 5;
        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];
            MemAccessCount += 2;
            if (cc_prev[u] < cc_curr[v])
            {

                cc_curr[v] = cc_prev[u];
                // changed = true;
                changed++;
            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}

/*fault tolerant SV sweep */
int FFSVSweep_Sync(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr, uint32_t* m_curr,
                   uint32_t* off, uint32_t* ind)
{
    int changed = 0;
    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        MemAccessCount += 6;
        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];
            MemAccessCount += 2;
            if (cc_prev[u] < cc_curr[v])
            {

                m_curr[v] = edge;
                cc_curr[v] = cc_prev[u];
                changed++;

            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}




// FaultFreeSVMain
lp_state_t FFWoSVAlg_Sync( graph_t *graph,
                           stat_t* stat       /*for counting stats of each iteration*/
                         )
{

    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;

    /*initialize */
    MemAccessCount = 0;

    lp_state_t lps_curr, lps_prev;
    alloc_lp_state(graph, &lps_curr);
    alloc_lp_state(graph, &lps_prev);

    init_lp_state(graph, &lps_curr);
    init_lp_state(graph, &lps_prev);


    uint32_t* cc_curr = lps_curr.CC;
    uint32_t* cc_prev = lps_prev.CC;

    bool changed;
    int num_changes;
    int num_corrections;
    size_t iteration = 0;
    do
    {
        long long prMemAccessCount = MemAccessCount;
        memcpy(cc_prev, cc_curr, numVertices * sizeof(uint32_t));
        tic();
        num_changes = FFWoSVSweep_Sync(numVertices, cc_prev, cc_curr, off, ind);
        stat->SvTime[iteration] = toc();
        stat->SvMemCount[iteration] = MemAccessCount - prMemAccessCount;
        iteration += 1;
    }
    while (num_changes);
    stat->numIteration = iteration;
#ifdef DEBUG
    printf("NUmber of iteration for fault free=%d\n", iteration );
#endif

    // free(cc_prev);
    // return cc_curr;

    free_lp_state(&lps_prev);
    return lps_curr;
}







lp_state_t FFSVAlg_Sync( graph_t *graph,
                         lp_state_t lps_curr,
                         stat_t* stat,       /*for counting stats of each iteration*/
                         int max_iter        /*contgrolling maximum number of iteration*/
                       )
{

    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;


    /*initialize */
    MemAccessCount = 0;
    lp_state_t lps_prev;

    alloc_lp_state(graph, &lps_prev);
    init_lp_state(graph, &lps_prev);


    uint32_t* cc_curr = lps_curr.CC;
    uint32_t* cc_prev = lps_prev.CC;

    uint32_t* m_curr = lps_curr.Ps;
    uint32_t* m_prev = lps_prev.Ps;


    bool changed;
    int num_changes;
    int num_corrections;
    size_t iteration = 0;
    do
    {
        long long prMemAccessCount = MemAccessCount;
        memcpy(cc_prev, cc_curr, numVertices * sizeof(uint32_t));
        memcpy(m_prev, m_curr, numVertices * sizeof(uint32_t));
        tic();
        num_changes = FFSVSweep_Sync(numVertices, cc_prev, cc_curr, m_curr, off, ind);
        // printf("Executing Iteration     %d: Changes =%d, Corrections=%d\n",
        //        iteration, num_changes, num_corrections );
        stat->SvTime[iteration] = toc();
        stat->SvMemCount[iteration] = MemAccessCount - prMemAccessCount;
        iteration += 1;
    }
    while (num_changes && iteration <= max_iter);
    stat->numIteration = iteration;
#ifdef DEBUG
    printf("NUmber of iteration for fault free=%d\n", iteration );
#endif

    // free(cc_prev);
    // return cc_curr;
    free_lp_state(&lps_prev);
    return lps_curr;
}