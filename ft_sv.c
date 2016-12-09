
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
#include "sssv.h"









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







static inline uint32_t FaultInjectWord(uint32_t value)
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
            // if(0)
            {
                /* code */
                u = FaultInjectWord(uT);

                int num_trials=0;
                while (u >= nv )    /*a better check can be used*/
                {
                    uT = vind[edge];
                    MemAccessCount++;

                    u = FaultInjectWord(uT);
                    num_trials++;
                    if (num_trials==5)
                    {
                        /* code */
                        u = uT;
                        break;
                    }
                    // printf("stuck 1 %d %d %d, edge=%d, vind[edge]=%d\n",v,u, uT,edge,vind[edge]);
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
                // if(0)
                {
                    do
                    {
                        var = FaultInjectWord(cc_prev_u);
                        // printf("stuck 2 %u %u\n", var, u);
                    }
                    while (var > u);
                    // while(0);
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

/*fault tolerant SV Async sweep */
int  TMSVSweep_Async(graph_t *graph, lp_state_t *lp_state,
                     uint32_t* FaultArrEdge,         /*probability of bit flip*/
                     uint32_t* FaultArrCC )        /*probability of bit flip for type-2 faults*/
{
    size_t nv = graph->numVertices;
    uint32_t* off = graph->off;
    uint32_t* ind = graph->ind;
    uint32_t* component_map = lp_state->CC;
    uint32_t* CC = lp_state->CC;

    int  changed = 0;

    // SV Sweep with faults
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

                /* code */
                // printf("//v=6  u=%d  uT=%d\n", u, uT );

            }
            else
            {
                u = uT;
            }

            uint32_t cc_prev_u = CC[u];
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
                    var = FaultInjectWord(cc_prev_u);
                    do
                    {
                        var = FaultInjectWord(cc_prev_u);
                        // printf("stuck 2 %u %u\n", var, u);
                    }
                    while (var > u);

                    /* code */
                    // printf("//v=%d  CC[u]=%d  CC[u]T=%d\n",v, cc_prev_u, var );

                }
                else
                {
                    var = cc_prev_u;
                }

                cc_prev_u = var;
            }

            if (cc_prev_u < CC[v])
            {
                lp_state->Ps[v] = edge;
                CC[v] = cc_prev_u;
                changed = 1;
            }
        }
    }

    /*shortcutting goes here*/
    return changed;

}
int  TMSVSweep_Sync(graph_t *graph, 
                     lp_state_t* lp_state_in,      // State which is read in the iteration
                     lp_state_t* lp_state_out,     // State which is written to in the iteration     
                     uint32_t* FaultArrEdge,       /*probability of bit flip*/
                     uint32_t* FaultArrCC )        /*probability of bit flip for type-2 faults*/
{
    size_t nv = graph->numVertices;
    uint32_t* off = graph->off;
    uint32_t* ind = graph->ind;
    uint32_t* CC = lp_state_in->CC;

    int  changed = 0;

    // SV Sweep with faults
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

            uint32_t cc_prev_u = CC[u];
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
                    var = FaultInjectWord(cc_prev_u);
                    if ( var<cc_prev_u)

                    {
                        //printf("// Error injected %d %d\n",CC[u], var );
                    }
                }
                else
                {
                    var = cc_prev_u;
                }

                cc_prev_u = var;
            }

            // Write to the auxiliary copy
            if (cc_prev_u < lp_state_out->CC[v])
            {
                lp_state_out->CC[v] = cc_prev_u;
                changed = 1;
            }
        }
    }

    /*shortcutting goes here*/
    return changed;

}

int FISVModAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                     stat_t* stat, int max_iter )
// fault injected label propagation algorithm;
// will give incorrect output (most likely)
// used to measure failure rate
// Correction based on Triple Modular Algorithm
{
    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;

    // Allocate auxillary state
    lp_state_t lp_state_aux1, lp_state_aux2, lp_state_aux3, lp_state_aux4;
    alloc_lp_state(graph, &lp_state_aux1);
    init_lp_state(graph, &lp_state_aux1);
    alloc_lp_state(graph, &lp_state_aux2);
    init_lp_state(graph, &lp_state_aux2);
    alloc_lp_state(graph, &lp_state_aux3);
    init_lp_state(graph, &lp_state_aux3);

    lp_state_t* lp_state_A = &lp_state_aux1;
    lp_state_t* lp_state_B = &lp_state_aux2;
    lp_state_t* lp_state_C = &lp_state_aux3;

    alloc_lp_state(graph, &lp_state_aux4);
    init_lp_state(graph, &lp_state_aux4);
    lp_state_t* lp_state_cur = &lp_state_aux4;

    /*get fault probability*/
    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);


    uint32_t* AFaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* AFaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* BFaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* BFaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* CFaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* CFaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));


    int  changed;
    size_t iteration = 0;
    int corrupted;
    do
    {
        char label[100];
        int votes_failed;        
        int vote_iter = 0;
        changed = 0;

        do 
        {
            /*intialize fault array*/
            for (int i = 0; i < numEdges; ++i)
            {
                /* code */
                AFaultArrEdge[i] = 0;
                AFaultArrCC[i] = 0;
                BFaultArrEdge[i] = 0;
                BFaultArrCC[i] = 0;
                CFaultArrEdge[i] = 0;
                CFaultArrCC[i] = 0;
            }
    
            int numEdgeFault = 0;
            int numCCFault = 0;
    
            while (numEdgeFault <  0.5 * fProb1 * numEdges)
            {
                uint32_t ind = rand() % numEdges;
                AFaultArrEdge[ind] = 1;

                ind = rand() % numEdges;
                BFaultArrEdge[ind] = 1;

                ind = rand() % numEdges;
                CFaultArrEdge[ind] = 1;

                numEdgeFault++;
    
            }
    
            while (numCCFault <  0.5 * fProb2 * numEdges)
            {
                uint32_t ind = rand() % numEdges;
                AFaultArrCC[ind] = 1;

                ind = rand() % numEdges;
                BFaultArrCC[ind] = 1;

                ind = rand() % numEdges;
                CFaultArrCC[ind] = 1;

                numCCFault++;
    
            }

            corrupted = 0;
            sprintf(label, "Iteration_%d", iteration);
            printParentTree(label, graph, lp_state_prev);

            votes_failed = 0;
            
            TMSVSweep_Sync(graph, lp_state_prev, lp_state_A, AFaultArrEdge, AFaultArrCC) ;
            TMSVSweep_Sync(graph, lp_state_prev, lp_state_B, BFaultArrEdge, BFaultArrCC) ;
            TMSVSweep_Sync(graph, lp_state_prev, lp_state_C, CFaultArrEdge, CFaultArrCC) ;

            for (int i=0; i < numVertices && !votes_failed; i++)
            {
                if(lp_state_A->CC[i] == lp_state_B->CC[i])
                {
                    lp_state_cur->CC[i] = lp_state_A->CC[i];
                }
                else if(lp_state_A->CC[i] == lp_state_C->CC[i])
                {
                    lp_state_cur->CC[i] = lp_state_A->CC[i];
                }
                else if(lp_state_C->CC[i] == lp_state_B->CC[i])
                {
                    lp_state_cur->CC[i] = lp_state_C->CC[i];
                }
                else
                {
                    // Recount
                    votes_failed = 1;
                }
            }
            //printf("//Finished voting iteration %d\n", vote_iter);
            vote_iter++;
        }
        while (votes_failed && vote_iter+iteration < max_iter);

        if(vote_iter >= max_iter) 
        {
            // printf("\nVotes couldn't agree within maximum allowed iterations");
            iteration = max_iter;
        }
        iteration += vote_iter;
        sprintf(label, "Iteration_%d_after", iteration);
        printParentTree(label, graph, lp_state_cur);

        // Check convergence
        for(int ii=0; ii < numVertices; ii++)
            if(lp_state_prev->CC[ii] != lp_state_cur->CC[ii]) changed = 1;
        
        // Copy over current state to previous
        memcpy(lp_state_prev->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));

        // Copy over current state to A, B, C
        memcpy(lp_state_A->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));
        memcpy(lp_state_B->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));
        memcpy(lp_state_C->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));

        //printf("//Finished iteration  %d\n", iteration );
    }
    while ((changed || corrupted) && iteration<max_iter);

    /*updating stats*/
    stat->numIteration = iteration;
    free (AFaultArrEdge);
    free (AFaultArrCC);
    free (BFaultArrEdge);
    free (BFaultArrCC);
    free (CFaultArrEdge);
    free (CFaultArrCC);
    printf("// Triple Modular Sync: Number of iteration is %d\n", iteration );

    // Free LP aux
    free_lp_state(lp_state_A);
    free_lp_state(lp_state_B);
    free_lp_state(lp_state_C);
    free_lp_state(lp_state_cur);

    return 0;
}

int FISVModAlg_Async( lp_state_t* lp_state_prev, graph_t *graph,
                     stat_t* stat, int max_iter )
// fault injected label propagation algorithm;
// will give incorrect output (most likely)
// used to measure failure rate
// Correction based on Triple Modular Algorithm
{
    size_t numVertices  = graph->numVertices;
    size_t numEdges  = graph->numEdges;
    uint32_t* off  = graph->off;
    uint32_t* ind  = graph->ind;

    // Allocate auxillary state
    lp_state_t lp_state_aux1, lp_state_aux2;
    alloc_lp_state(graph, &lp_state_aux1);
    init_lp_state(graph, &lp_state_aux1);
    alloc_lp_state(graph, &lp_state_aux2);
    init_lp_state(graph, &lp_state_aux2);

    lp_state_t* lp_state_A = lp_state_prev;
    lp_state_t* lp_state_B = &lp_state_aux1;
    lp_state_t* lp_state_C = &lp_state_aux2;

    /*get fault probability*/
    double fProb1, fProb2;

    getFault_prob(&fProb1, &fProb2);


    uint32_t* AFaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* AFaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* BFaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* BFaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* CFaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* CFaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));


    int  changed;
    size_t iteration = 0;
    int corrupted;
    do
    {
        char label[100];
        changed = 0;

        /*intialize fault array*/
        for (int i = 0; i < numEdges; ++i)
        {
            /* code */
            AFaultArrEdge[i] = 0;
            AFaultArrCC[i] = 0;
            BFaultArrEdge[i] = 0;
            BFaultArrCC[i] = 0;
            CFaultArrEdge[i] = 0;
            CFaultArrCC[i] = 0;
        }
        
        int numEdgeFault = 0;
        int numCCFault = 0;
        
        while (numEdgeFault <  0.5 * fProb1 * numEdges)
        {
            uint32_t ind = rand() % numEdges;
            AFaultArrEdge[ind] = 1;
        
            ind = rand() % numEdges;
            BFaultArrEdge[ind] = 1;
        
            ind = rand() % numEdges;
            CFaultArrEdge[ind] = 1;
        
            numEdgeFault++;
        
        }
        
        while (numCCFault <  0.5 * fProb2 * numEdges)
        {
            uint32_t ind = rand() % numEdges;
            AFaultArrCC[ind] = 1;
        
            ind = rand() % numEdges;
            BFaultArrCC[ind] = 1;
        
            ind = rand() % numEdges;
            CFaultArrCC[ind] = 1;
        
            numCCFault++;
        
        }
        
        corrupted = 0;
        sprintf(label, "Iteration_%d", iteration);
        printParentTree(label, graph, lp_state_A);
        
        int chgA = TMSVSweep_Async(graph, lp_state_A, AFaultArrEdge, AFaultArrCC) ;
        int chgB = TMSVSweep_Async(graph, lp_state_B, BFaultArrEdge, BFaultArrCC) ;
        int chgC = TMSVSweep_Async(graph, lp_state_C, CFaultArrEdge, CFaultArrCC) ;

        int converged = 0;
        iteration += 1;

        // Convergence check
        if (chgA == 0 && chgB == 0)
        {
            converged = 1;
            for (int i = 0; i < numVertices; i++)
            {
                if (lp_state_A->CC[i] != lp_state_B->CC[i])
                {
                    converged = 0;
                    break;
                }
            }
        }

        if (converged) break;
        
        if (chgA == 0 && chgC == 0)
        {
            converged = 1;
            for (int i = 0; i < numVertices; i++)
            {
                if (lp_state_A->CC[i] != lp_state_C->CC[i])
                {
                    converged = 0;
                    break;
                }
            }
        }

        if (converged) break;

        if (chgC == 0 && chgB == 0)
        {
            converged = 1;
            for (int i = 0; i < numVertices; i++)
            {
                if (lp_state_C->CC[i] != lp_state_B->CC[i])
                {
                    converged = 0;
                    break;
                }
            }
            lp_state_A = lp_state_C;
        }

        if (converged) break;

        // Voting Process
        for (int i=0; i < numVertices; i++)
        {
            if(lp_state_A->CC[i] == lp_state_B->CC[i])
            {
                lp_state_A->CC[i] = lp_state_A->CC[i];
            }
            else if(lp_state_A->CC[i] == lp_state_C->CC[i])
            {
                lp_state_A->CC[i] = lp_state_A->CC[i];
            }
            else if(lp_state_C->CC[i] == lp_state_B->CC[i])
            {
                lp_state_A->CC[i] = lp_state_C->CC[i];
            }
            else
            {
                // Reset vertex 
                lp_state_A->CC[i] = i;
            }
        }

        sprintf(label, "Iteration_%d_after", iteration);
        printParentTree(label, graph, lp_state_A);

        // Copy over current state to A, B, C
        memcpy(lp_state_B->CC, lp_state_A->CC, numVertices * sizeof(uint32_t));
        memcpy(lp_state_C->CC, lp_state_A->CC, numVertices * sizeof(uint32_t));

        //printf("//Finished iteration  %d\n", iteration );
    }
    while (iteration<max_iter);

    // Set output
    memcpy(lp_state_prev->CC, lp_state_A->CC, numVertices * sizeof(uint32_t));

    /*updating stats*/
    stat->numIteration = iteration;
    free (AFaultArrEdge);
    free (AFaultArrCC);
    free (BFaultArrEdge);
    free (BFaultArrCC);
    free (CFaultArrEdge);
    free (CFaultArrCC);
    printf("// Triple Modular Async: Number of iteration is %d\n", iteration );

    // Free LP aux
    free_lp_state(lp_state_B);
    free_lp_state(lp_state_C);

    return 0;
}
