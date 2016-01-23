
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <string.h>

#include "stat.h"

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



uint32_t* BaselineSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
                          stat_t* stat       /*for counting stats of each iteration*/
                        )
{


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

    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];

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




uint32_t* FaultFreeSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
  stat_t* stat       /*for counting stats of each iteration*/
                        )
{
    /*initialize */
    

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
        /*swap the pointer*/
        // uint32_t* tmp = cc_curr;
        // cc_curr = cc_prev;
        // cc_prev = tmp;

        memcpy(cc_prev,cc_curr,numVertices*sizeof(uint32_t));

        num_changes = FaultFreeSVSweep(numVertices, cc_prev, cc_curr, off, ind);
        printf("Executing Iteration     %d: Changes =%d, Corrections=%d\n",
               iteration, num_changes, num_corrections );

        iteration += 1;
    }
    while (num_changes);

    printf("NUmber of iteration for fault free=%d\n", iteration );

    free(cc_prev);

    return cc_curr;
}
