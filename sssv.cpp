// This file contains implementation for tarzen's algorithm for strongly connected component in the graph
#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include "sv.h"
#include "sssv.h"


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
        if (v == CC[v])
        {
            /* code */
            lp_state->Ps[v] = -1;
        }
    }

    printf("finish self-stabilizing Shortcutting %d\n", iteration);


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

        lps_curr->CC[v] = MIN(v,CC_prev[v]);
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


    ssShortcut_Async(graph, lp_state);

    return 0;
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

    return 0;
}


