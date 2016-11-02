
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <math.h>
#include "sv.h"
#include "timer.h"
#include "faultInjection.h"


#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "sv.h"









// uncomment following for debug statements
// #define DEBUG

// the variable to keep count of number of memory accesses
static long long MemAccessCount;



/*checks for BadAdjacency type faults and if possible
corrects it*/
int SCstep_Sync(size_t nv,
                uint32_t* cc_curr, uint32_t* cc_prev,
                uint32_t* m_curr, uint32_t* m_prev,
                uint32_t* off, uint32_t* ind)
{
    int corrections = 0;
    int changes = 0;

    /*now correcting bad parent*/
    for (size_t v = 0; v < nv; v++)
    {
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        MemAccessCount += 6;
        MemAccessCount += 3;
        // check if the modifier is correct
        if (m_curr[v] == -1)
        {
            /* code */
            if (cc_curr[v] != v)
            {
                /* code */
#ifdef DEBUG
                printf("1.Error detected - BadParent %d.... correcting\n", v);
#endif
                /*do the correction*/
                corrections++;
                for (size_t edge = 0; edge < vdeg; edge++)
                {
                    const uint32_t u = vind[edge];
                    MemAccessCount += 2;
                    if (cc_prev[u] < cc_curr[v])
                    {
                        m_curr[v] = edge;
                        cc_curr[v] = cc_prev[u];
                        // changed = true;
                    }
                }

            }
        }
        else if (cc_curr[v] != cc_prev[vind[m_curr[v]]] )
        {
            /* code */
            cc_curr[v] = cc_prev[vind[m_curr[v]]];
#ifdef DEBUG
            printf("2.Error detected - BadParent %d.... correcting\n", v);
#endif
            corrections++;
            /*do the exact correction*/

            // if(0)
            for (size_t edge = 0; edge < vdeg; edge++)
            {
                const uint32_t u = vind[edge];
                MemAccessCount += 2;
                if (cc_prev[u] < cc_curr[v])
                {
                    m_curr[v] = edge;
                    cc_curr[v] = cc_prev[u];

                    // changed = true;
                }
            }

        }

        if (cc_curr[v] > v )
        {
            /* code */
            cc_curr[v] = v;
#ifdef DEBUG
            printf("3.Error detected - BadParent %d.... correcting\n", v);
#endif
            // if(v==6085)printf("correcting for v=%d, cc[v] =%d\n",v,cc_curr[v] );
            /*do the correction*/
            corrections++;
            for (size_t edge = 0; edge < vdeg; edge++)
            {
                const uint32_t u = vind[edge];
                MemAccessCount += 2;
                if (cc_prev[u] < cc_curr[v])
                {
                    m_curr[v] = edge;
                    cc_curr[v] = cc_prev[u];

                }
            }

        }

        if (cc_curr[v] != cc_prev[v] )
        {
            changes++;
        }

    }

    // return corrections;
    return changes;
}







uint32_t FaultInjectWord(uint32_t value)
{

    uint32_t mask = 0;

    mask += (1 << rand() % 32);
    return mask ^ (value);

}


int FISVSweep_Sync(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                           uint32_t* m_curr, uint32_t* off, uint32_t* ind,
                           uint32_t* FaultArrEdge,         /*probability of bit flip*/
                           uint32_t* FaultArrCC         /*probability of bit flip for type-2 faults*/
                          )
{
    int changed = 0;

    for (size_t v = 0; v < nv; v++)
    {
        // printf("vertex %u\n", v);
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];
        MemAccessCount += 5;
        for (size_t edge = 0; edge < vdeg; edge++)
        {
            uint32_t uT = vind[edge];
            uint32_t u;
            MemAccessCount++;

            if (FaultArrEdge[off[v] + edge])
            {
                /* code */
                u = FaultInjectWord(uT);

                while (u >= nv)    /*a better check can be used*/
                {
                    uT = vind[edge];
                    MemAccessCount++;

                    u = FaultInjectWord(uT);
                    // printf("stuck 1\n");
                }
            }
            else
            {
                u = uT;
            }




            uint32_t cc_prev_u = cc_prev[u];
            uint32_t var;
            MemAccessCount++;
            if (u == 0)
            {
                /* code */
                cc_prev_u = 0;
            }
            else
            {
                if (FaultArrCC[off[v] + edge])
                {
                    do
                    {
                        var = FaultInjectWord(cc_prev_u);
                        // printf("stuck 2 %u %u\n", var, u);
                    }
                    while (var > u);
                }
                else
                {
                    var = cc_prev_u;
                }

                cc_prev_u = var;
            }

            if (cc_prev_u < cc_curr[v])
            {
                m_curr[v] = edge;
                cc_curr[v] = cc_prev_u;
                changed++;
                MemAccessCount++;
                if (cc_prev_u != cc_prev[u])
                {

                    for (int edge = 0; edge < vdeg; ++edge)
                    {

                        uint32_t u = vind[edge];

                    }

                }


            }
        }
    }

    /*shortcutting goes here*/
    return changed;

}







lp_state_t SCSVAlg_Sync( graph_t *graph,
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

    /*get fault probability*/
    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);

    lp_state_t lps_curr, lps_prev;
    alloc_lp_state(graph, &lps_curr);
    alloc_lp_state(graph, &lps_prev);

    init_lp_state(graph, &lps_curr);
    init_lp_state(graph, &lps_prev);


    uint32_t* cc_curr = lps_curr.CC;
    uint32_t* cc_prev = lps_prev.CC;

    uint32_t* m_curr = lps_curr.Ps;
    uint32_t* m_prev = lps_prev.Ps;
    uint32_t* FaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* FaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));



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


        for (int i = 0; i < numEdges; ++i)
        {
            /* code */
            FaultArrEdge[i] = 0;
            FaultArrCC[i] = 0;
        }

        int numEdgeFault = 0;
        int numCCFault = 0;

        while (numEdgeFault <  0.5 * fProb1 * numEdges)
        {
            uint32_t ind = rand() % numEdges;
            FaultArrEdge[ind] = 1;
            numEdgeFault++;

        }

        while (numCCFault <  0.5 * fProb2 * numEdges)
        {
            uint32_t ind = rand() % numEdges;
            FaultArrCC[ind] = 1;
            numCCFault++;

        }




        num_changes = FISVSweep_Sync(numVertices,
                                             cc_prev, cc_curr, m_curr,
                                             off, ind,
                                             FaultArrEdge, FaultArrCC);


        stat->SvTime[iteration] = toc();
        stat->SvMemCount[iteration] = MemAccessCount - prMemAccessCount;

        prMemAccessCount = MemAccessCount;
        tic();
        num_corrections = SCstep_Sync(numVertices, cc_curr, cc_prev,
                                      m_curr, m_prev, off, ind);

        stat->FtTime[iteration] = toc();
        stat->FtMemCount[iteration] = MemAccessCount - prMemAccessCount;
        stat->NumChanges[iteration] = num_changes;
        stat->NumCorrection[iteration] = num_corrections;

#ifdef DEBUG
        printf("Executing Iteration     %d: Changes =%d, Corrections=%d\n",
               iteration, num_changes, num_corrections );
#endif
        iteration += 1;
    } /*do loop ends here*/
    while (num_corrections > 0);



#ifdef DEBUG
    printf("NUmber of iteration for fault free=%d\n", iteration );
#endif
    stat->numIteration = iteration;


    
    free (FaultArrEdge);
    free (FaultArrCC);
    free_lp_state(&lps_prev);
    return lps_curr;
}