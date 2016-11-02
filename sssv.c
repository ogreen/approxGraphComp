#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <malloc.h>
#include <math.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


#include "sv.h"
#include "sssv.h"
#include "faultInjection.h"




#define MIN(a,b) a>b?b:a












int ssShortcut_Async(graph_t *graph, lp_state_t*lp_state)
// performs shortcutting and cycle detection togather
{

    size_t nv = graph->numVertices;
    uint32_t*CC = lp_state->CC;
    uint32_t*P = lp_state->P;
    int numChanges = 1;
    int iteration = 0;

    for (uint32_t v = 0; v < nv; v++)
    {
        uint32_t Pv = lp_state->Ps[v] == -1 ? v : graph->ind[graph->off[v]  + lp_state->Ps[v]];
        lp_state->P[v] = Pv;
        lp_state->CC[v] = Pv; // init to parent
        if (v == 3615)
        {
            printf("Check0: %d  %d %d\n", v, CC[v], P[v] );
        }

    }

    while (numChanges)
    {
        iteration++;
        // printf("iteration %d\n",iteration );
        numChanges = 0;
        for (uint32_t v = 0; v < nv; v++)
        {
            if (v == 3615)
            {
                printf("Check1: %d  %d %d\n", v, CC[v], P[v] );
            }
            uint32_t tmp = MIN(CC[v], CC[P[v]]);
            if (CC[v] != tmp)
            {
                if (v == 3615)
                {
                    printf("Check2: %d  %d %d\n", v, CC[v], P[v] );
                }
                numChanges++;

                // printf("%d %d %d \n",v, CC[v], tmp );
                CC[v] = MIN(CC[v], CC[P[v]]);

                // printf("%d %d %d %d\n",i,M1[i], M[i],M[A[i]]);
            }

        }

        for (uint32_t v = 0; v < nv; v++)
        {
            P[v] =  P[P[v]];
        }

    }

    // loop detect and correct
    int loop=0;
    for (uint32_t v = 0; v < nv; v++)
    {
        
        if (v == CC[v])
        {
            if(lp_state->Ps[v] != -1 )
            {
                printf("Loop found %d\n",v );
                loop++;
                lp_state->Ps[v] = -1;
            }
            
            
        }
        CC[v] = MIN(v, CC[v]);
    }

    printf("finish self-stabilizing Shortcutting %d\n", iteration);
    return loop;

}


int ssShortcut_Sync(graph_t *graph, lp_state_t*lps_curr, lp_state_t*lps_prev)
// performs shortcutting and cycle detection togather
// lps_prev has the input state
// lps_curr will have the output state
{
    size_t nv = graph->numVertices;
    uint32_t*CC = lps_curr->CC;
    uint32_t*P = lps_curr->P;
    int numChanges = 1;
    int iteration = 0;

    for (uint32_t v = 0; v < nv; v++)
    {
        uint32_t Pv = lps_prev->Ps[v] == -1 ? v : graph->ind[graph->off[v]  + lps_prev->Ps[v]];
        lps_prev->P[v] = Pv;
        lps_prev->CC[v] = Pv; // init to parent

    }

    uint32_t *CC_prev, *CC_curr, *P_curr, *P_prev;
    CC_prev = lps_prev->CC;
    P_prev = lps_prev->P;
    CC_curr = lps_curr->CC;
    P_curr = lps_curr->P;


    while (numChanges)
    {
        iteration++;
        // printf("iteration %d\n",iteration );
        numChanges = 0;
        for (uint32_t v = 0; v < nv; v++)
        {
            uint32_t tmp = MIN(CC_prev[v], CC_prev[P_prev[v]]);
            if (CC_prev[v] != tmp)
            {
                numChanges++;

                // printf("%d %d %d \n",v, CC[v], tmp );
                CC_curr[v] = MIN(CC_prev[v], CC_prev[P_prev[v]]);

                // printf("%d %d %d %d\n",i,M1[i], M[i],M[A[i]]);
            }
            else
            {
                CC_curr[v] = CC_prev[v];
            }

        }

        for (uint32_t v = 0; v < nv; v++)
        {
            P_curr[v] =  P_prev[P_prev[v]];
        }

        uint32_t* tmp;
        tmp = CC_curr;
        CC_curr = CC_prev;
        CC_prev = tmp;

        tmp = P_curr;
        P_curr = P_prev;
        P_prev = tmp;


    }

    /*both will have the output*/
    // if (iteration % 2 == 0)
    for (uint32_t v = 0; v < nv; v++)
    {

        lps_curr->CC[v] = MIN(v, CC_prev[v]);
    }

    // if(1)
    // {

    //     memcpy(P_curr, P_prev, nv * sizeof(uint32_t));
    //     memcpy(CC_curr, CC_prev, nv * sizeof(uint32_t));
    // }


    // loop detect and correct
    // and finally set the minimum
    for (uint32_t v = 0; v < nv; v++)
    {

        // lps_curr->CC[v] = MIN(v,lps_curr->CC[v]);

        if (v == CC[v])
        {
            /* code */
            lps_curr->Ps[v] = -1;
        }
        else
        {
            lps_curr->Ps[v] = lps_prev->Ps[v];
        }

    }

    printf("finish self-stabilizing Shortcutting %d\n", iteration);


}





int SSstep_Async(graph_t *graph, lp_state_t*lp_state)
// detects cycle in the linked list and resolve the cycle issues
/*makes the arbitrary step lp_state of label propagation algorithm
self-stabilizing; i.e.
lp_state<-SS(lp_state, graph)
LP(lp_state) -> correct solution
*/
{


    size_t nv = graph->numVertices;
    uint32_t*CC = lp_state->CC;
    /*first check following condition for all vertex

    1.  P[v] \in N(v) => Ps[v]+2>=1 & Ps[v] < |adj(v)|
    2.  CC[v] >= CC[P[v]]
    */
    uint32_t v = 3615;
    if (v == 3615)
    {
        printf("Check0: %d  %d \n", v, CC[v] );
    }
    int corrupted = 0;
    for (uint32_t v = 0; v < nv; v++)
    {
        const size_t vdeg = graph->off[v + 1] - graph->off[v];


        if (CC[v] > v) // should be greater than
        {
            /* code */
            CC[v] = v;
            lp_state->Ps[v] = -1;
            // mark the vertex as corrupted
            if (v == 3615)
            {
                printf("Check1: %d  %d \n", v, CC[v] );
            }

            corrupted++;

        }

        else if ((lp_state->Ps[v] >= vdeg && lp_state->Ps[v] != -1) || lp_state->Ps[v] + 2 < 1)
        {

            if (v == 3615)
            {
                printf("Check2: %d  %d \n", v, CC[v] );
            }
            /* reset that node */
            lp_state->Ps[v] = -1;
            lp_state->CC[v] = v;

            corrupted++;

        }
        else
        {
            if (lp_state->Ps[v] == -1 )
            {
                if (v == 3615)
                {
                    printf("Check3: %d  %d \n", v, CC[v] );
                }
                /* code */
                if (lp_state->CC[v] != v)
                {
                    lp_state->CC[v] = v;

                    corrupted++;

                }
            }
            else
            {

                uint32_t Pv = graph->ind[graph->off[v]  + lp_state->Ps[v]];
                if (lp_state->CC[v] < lp_state->CC[Pv])
                {
                    if (v == 3615)
                    {
                        printf("Check4: %d  %d \n", v, CC[v] );
                    }
                    /* reset that node */
                    lp_state->Ps[v] = -1;
                    lp_state->CC[v] = v;
                    // mark it as incorrect

                    corrupted++;

                }
            }
        }


    }


    if (v == 3615)
    {
        printf("Check5: %d  %d \n", v, CC[v] );
    }
    /*Now do the Cycle Correction*/

    printf("NUmber of corruptions is %d, NV %d\n", corrupted, nv );


    corrupted += ssShortcut_Async(graph, lp_state);
    if (v == 3615)
    {
        printf("Check0: %d  %d \n", v, CC[v] );
    }

    return corrupted;
}


int SSstep_Sync(graph_t *graph, lp_state_t*lp_state_out, lp_state_t* lp_state_in )
// detects cycle in the linked list and resolve the cycle issues
/*
Self stabilization step for Sync algorithm(2 copies) with aux being temp array;
lp_state_out<-SS(lp_state_in, graph)
LP(lp_state) -> correct solution
*/
{


    size_t nv = graph->numVertices;
    uint32_t*CC = lp_state_in->CC;
    /*first check following condition for all vertex

    1.  P[v] \in N(v) => Ps[v]+2>=1 & Ps[v] < |adj(v)|
    2.  CC[v] >= CC[P[v]]
    */
    int corrupted = 0;
    for (uint32_t v = 0; v < nv; v++)
    {
        const size_t vdeg = graph->off[v + 1] - graph->off[v];

        if (CC[v] > v) // should be greater than
        {
            /* code */
            CC[v] = v;
            lp_state_in->Ps[v] = -1;
            // mark the vertex as corrupted

            corrupted++;

        }

        else if ((lp_state_in->Ps[v] >= vdeg && lp_state_in->Ps[v] != -1) || lp_state_in->Ps[v] + 2 < 1)
        {

            /* reset that node */
            lp_state_in->Ps[v] = -1;
            lp_state_in->CC[v] = v;

            corrupted++;

        }
        else
        {
            if (lp_state_in->Ps[v] == -1 )
            {
                /* code */
                if (lp_state_in->CC[v] != v)
                {
                    lp_state_in->CC[v] = v;

                    corrupted++;

                }
            }
            else
            {

                uint32_t Pv = graph->ind[graph->off[v]  + lp_state_in->Ps[v]];
                if (lp_state_in->CC[v] < lp_state_in->CC[Pv])
                {
                    /* reset that node */
                    lp_state_in->Ps[v] = -1;
                    lp_state_in->CC[v] = v;
                    // mark it as incorrect

                    corrupted++;

                }
            }
        }


    }


    /*Now do the Cycle Correction*/

    printf("NUmber of corruptions is %d, NV %d\n", corrupted, nv );


    ssShortcut_Sync(graph, lp_state_out, lp_state_in);

    return corrupted;
}


static inline uint32_t FaultInjectWord(uint32_t value)
{

    uint32_t mask = 0;

    mask += (1 << rand() % 32);
    return mask ^ (value);

}

static long long MemAccessCount;
/*fault tolerant SV sweep */
int  FISVSweep_Async(graph_t *graph, lp_state_t *lp_state,
                     uint32_t* FaultArrEdge,         /*probability of bit flip*/
                     uint32_t* FaultArrCC )        /*probability of bit flip for type-2 faults*/
{
    size_t nv = graph->numVertices;
    uint32_t* off = graph->off;
    uint32_t* ind = graph->ind;
    uint32_t* component_map = lp_state->CC;

    int  changed = 0;

    for (size_t v = 0; v < nv; v++)
    {
        uint32_t* vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

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

            uint32_t cc_prev_u = component_map[u];
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

            if (cc_prev_u < component_map[v])
            {
                lp_state->Ps[v] = edge;
                component_map[v] = cc_prev_u;
                changed = 1;
            }
        }
    }

    /*shortcutting goes here*/
    return changed;

}


int SSSVAlg_Async( lp_state_t *lp_state,  graph_t *graph,
                   stat_t* stat, int ssf // frequency of self stabilization
                 )
{
    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;

    /*get fault probability*/
    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);

    uint32_t* FaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* FaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));


    int  changed;
    size_t iteration = 0;
    int corrupted;
    do
    {
        /*intialize fault array*/
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

        corrupted = 0;
        changed = FISVSweep_Async(graph, lp_state, FaultArrEdge, FaultArrCC) ;
        iteration += 1;
        if (iteration % ssf == 0 || !changed)
        {
            corrupted = SSstep_Async(graph, lp_state);
        }
    }
    while (changed || corrupted);

    /*updating stats*/
    stat->numIteration = iteration;
    free (FaultArrEdge);
    free (FaultArrCC);
    printf("Number of iteration is %d\n", iteration );

    return 0;
}

