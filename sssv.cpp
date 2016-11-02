// This file contains implementation for tarzen's algorithm for strongly connected component in the graph
#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include "sv.h"
#include "sssv.h"


#define MIN(a,b) a>b?b:a












int selfstabShortcut(graph_t *graph, lp_state_t*lp_state)
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

    }

    while (numChanges)
    {
        iteration++;
        // printf("iteration %d\n",iteration );
        numChanges = 0;
        for (uint32_t v = 0; v < nv; v++)
        {
            uint32_t tmp = MIN(CC[v], CC[P[v]]);
            if (CC[v] != tmp)
            {
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
    for (uint32_t v = 0; v < nv; v++)
    {
        if (v==CC[v])
        {
            /* code */
            lp_state->Ps[v] = -1;
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
            
            corrupted++;
            
        }

        else if ((lp_state->Ps[v] >= vdeg && lp_state->Ps[v] != -1) || lp_state->Ps[v] + 2 < 1)
        {

            /* reset that node */
            lp_state->Ps[v] = -1;
            lp_state->CC[v] = v;
            
            corrupted++;
            
        }
        else
        {
            if (lp_state->Ps[v] == -1 )
            {
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
                    /* reset that node */
                    lp_state->Ps[v] = -1;
                    lp_state->CC[v] = v;
                    // mark it as incorrect
                    
                    corrupted++;
                    
                }
            }
        }


    }
    

    /*Now do the Cycle Correction*/

    printf("NUmber of corruptions is %d, NV %d\n", corrupted, nv );
    
    
    selfstabShortcut(graph, lp_state);

    return 0;
}


