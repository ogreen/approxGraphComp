
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <string.h>

#include "timer.h"
#include "stat.h"

#include "sv.h"
static long long MemAccessCount;


bool BaselineSVSweep(size_t nv, uint32_t* component_map, uint32_t* off, uint32_t* ind)
{
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
                component_map[v] = component_map[u];
                changed = true;
            }
        }
    }

    /*shortcutting goes here*/
    return changed;

}



// uint32_t* BaselineSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
//                           stat_t* stat       /*for counting stats of each iteration*/
//                         )
uint32_t* BaselineSVMain( graph_t *graph,
                          stat_t* stat)
{
    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;


    uint32_t* components_map = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    /* Initialize level array */
    for (size_t i = 0; i < numVertices; i++)
    {
        components_map[i] = i;
    }

    bool changed;
    size_t iteration = 0;
    do
    {
        changed = BaselineSVSweep(numVertices, components_map, off, ind);
        iteration += 1;
    }
    while (changed);

    /*updating stats*/
    stat->numIteration = iteration;
    printf("Number of iteration is %d\n", iteration );

    return components_map;
}

/*fault tolerant SV sweep */
int FaultFreeSVSweep(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
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




uint32_t* FaultFreeSVMain( graph_t *graph,
                           // size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
                           stat_t* stat       /*for counting stats of each iteration*/
                         )
{

    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;

    /*initialize */
    MemAccessCount = 0;

    uint32_t* cc_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* cc_prev = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));

    /* Initialize level array */
    for (size_t i = 0; i < numVertices; i++)
    {
        cc_curr[i] = i;
        cc_prev[i] = i;
    }


    bool changed;
    int num_changes;
    int num_corrections;
    size_t iteration = 0;
    do
    {
        long long prMemAccessCount = MemAccessCount;
        memcpy(cc_prev, cc_curr, numVertices * sizeof(uint32_t));
        tic();
        num_changes = FaultFreeSVSweep(numVertices, cc_prev, cc_curr, off, ind);
        // printf("Executing Iteration     %d: Changes =%d, Corrections=%d\n",
        //        iteration, num_changes, num_corrections );
        stat->SvTime[iteration] = toc();
        stat->SvMemCount[iteration] = MemAccessCount - prMemAccessCount;
        iteration += 1;
    }
    while (num_changes);
    stat->numIteration = iteration;
#ifdef DEBUG
    printf("NUmber of iteration for fault free=%d\n", iteration );
#endif
    free(cc_prev);

    return cc_curr;
}



/*fault tolerant SV sweep */
int FTSVSweep(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr, uint32_t* m_curr,
              uint32_t* off, uint32_t* ind)
{
    int changed = 0;
    // #pragma omp parallel for
    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        MemAccessCount += 6;
        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];
            MemAccessCount += 2;
            // if(v==270) printf("%d \n",u  );
            if (cc_prev[u] < cc_curr[v])
            {

                m_curr[v] = edge;
                cc_curr[v] = cc_prev[u];
                // changed = true;
                changed++;
                // if(v==270) printf("changed for %d, cc[%d]=%d edge=%d\n", v,v,cc_curr[v],edge );
            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}




uint32_t* FTSVMain( graph_t *graph,
    // size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
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

    uint32_t* cc_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* cc_prev = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* m_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* m_prev = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));

    /* Initialize level array */
    for (size_t i = 0; i < numVertices; i++)
    {
        cc_curr[i] = i;
        cc_prev[i] = i;
        m_curr[i] = -1;    /*relative parent*/
    }


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
        num_changes = FTSVSweep(numVertices, cc_prev, cc_curr, m_curr, off, ind);
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

    free(cc_prev);

    return cc_curr;
}