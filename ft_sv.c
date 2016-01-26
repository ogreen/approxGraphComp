
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


// uncomment following for debug statements
// #define DEBUG

// the variable to keep count of number of memory accesses
static long long MemAccessCount;

/*checks for BadAdjacency type faults and if possible corrects it*/
int ftBadAdjacency(size_t nv,
                   uint32_t* cc_curr, uint32_t* cc_prev,
                   uint32_t* m_curr, uint32_t* m_prev,
                   uint32_t* off, uint32_t* ind)
{
    int corrections = 0;
    for (size_t v = 0; v < nv; v++)
    {
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        int32_t found = 0;

        if (m_curr[v] == v)
        {
            found = 1;
        }
        else
        {
            for (size_t edge = 0; edge < vdeg; edge++)
            {
                const uint32_t u = vind[edge];

                if (m_curr[v] == u)
                {
                    found = 1;
                    break;
                }
            }
        }
        if (found == 0)
        {
            corrections++;
#ifdef DEBUG
            printf("Error detected - BadAdjacency %d.... correcting\n", v);
#endif
            cc_curr[v] = cc_prev[v];
            m_curr[v] = m_prev[v];
        }
    }

    return corrections;
}


/*checks for BadAdjacency type faults and if possible
corrects it*/
int ftBadAdjacencyBadParent_RelParent(size_t nv,
                                      uint32_t* cc_curr, uint32_t* cc_prev,
                                      uint32_t* m_curr, uint32_t* m_prev,
                                      uint32_t* off, uint32_t* ind)
{
    int corrections = 0;

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
        // else if (cc_curr[v] != cc_prev[vind[m_curr[v]]] )
        else if (cc_curr[v] != cc_prev[vind[m_curr[v]]] )
        {
            /* code */
            cc_curr[v] = cc_prev[vind[m_curr[v]]];
// #ifdef DEBUG
            printf("2.Error detected - BadParent %d.... correcting\n", v);
// #endif
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

    return corrections;
}

/*detect for Bad parent type faults*/
int ftBadParent_old(size_t nv,
                    uint32_t* cc_curr, uint32_t* cc_prev,
                    uint32_t* m_curr, uint32_t* m_prev,
                    uint32_t* off, uint32_t* ind)
{
    for (size_t v = 0; v < nv; v++)
    {
        if (m_curr[v] == v )
            continue;
        if (m_curr[m_prev[v]] == v )
        {
#ifdef DEBUG
            printf("Error detected at %d: Bad Parent... correcting by resetting\n", v);
#endif
            // uint32_t vp = m_curr[v];     /*error occured here*/
            const uint32_t *restrict vind = &ind[off[v]];
            const size_t vdeg = off[v + 1] - off[v];

            for (size_t edge = 0; edge < vdeg; edge++)
            {

                const uint32_t u = vind[edge];
                printf("%d  ", u );
                cc_curr[u] = u;
                m_curr[u] = u;

            }
            printf("\n");

            cc_curr[v] = v;
            m_curr[v] = v;


        }
    }
    return 0;
}

/*detect for Bad parent type faults*/
int ftBadParent(size_t nv,
                uint32_t* cc_curr, uint32_t* cc_prev,
                uint32_t* m_curr, uint32_t* m_prev,
                uint32_t* off, uint32_t* ind)
{
    int corrections = 0;
    for (size_t v = 0; v < nv; v++)
    {
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        int32_t found = 0;

        if (m_curr[v] == v)
        {
            continue;
        }
        else
        {
            /*now check for the second fault*/
            if (cc_curr[v] != cc_prev[m_curr[v]])
                // if (cc_curr[v] < cc_prev[m_curr[v]])
            {
                /* code */
                corrections++;
#ifdef DEBUG
                printf("Error detected at %d: Bad Parent... correcting by resetting\n", v);
#endif
                cc_curr[v] = cc_prev[m_curr[v]];

            }

        }

    }
    return corrections;
}


/*detect for Bad parent type faults: it corrects the detected fault by sweeping for exact*/
int ftBadParentExact(size_t nv,
                     uint32_t* cc_curr, uint32_t* cc_prev,
                     uint32_t* m_curr, uint32_t* m_prev,
                     uint32_t* off, uint32_t* ind)
{
    int corrections = 0;
    for (size_t v = 0; v < nv; v++)
    {
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        int32_t found = 0;

        if (m_curr[v] == v)
        {
            continue;
        }
        else
        {
            /*now check for the second fault*/
            // if (cc_curr[v] != cc_prev[m_curr[v]])
            if (cc_curr[v] < cc_prev[m_curr[v]])
            {
                corrections++;
                cc_curr[v] = cc_prev[m_curr[v]];
                /* code */
#ifdef DEBUG
                printf("Error detected at %d: Bad Parent... correcting by finding correct neighbour\n", v);
#endif
                const uint32_t *restrict vind = &ind[off[v]];
                const size_t vdeg = off[v + 1] - off[v];

                for (size_t edge = 0; edge < vdeg; edge++)
                {
                    const uint32_t u = vind[edge];

                    if (cc_prev[u] < cc_curr[v])
                    {
                        m_curr[v] = u;
                        cc_curr[v] = cc_prev[u];
                        // changed = true;
                    }
                }

            }

        }

    }
    return corrections;
}



/*fault tolerant SV sweep */
int FaultTolerantSVSweep(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                         uint32_t* m_curr, uint32_t* off, uint32_t* ind)
{
    int changed = false;

    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];

            if (cc_prev[u] < cc_curr[v])
            {
                m_curr[v] = u;
                cc_curr[v] = cc_prev[u];
                changed = true;
            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}

/*fault tolerant SV sweep */
int FaultTolerantSVSweep_RelParent(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                                   uint32_t* m_curr, uint32_t* off, uint32_t* ind)
{
    int changed = false;

    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];

            if (cc_prev[u] < cc_curr[v])
            {
                m_curr[v] = edge;
                cc_curr[v] = cc_prev[u];
                changed = true;
            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}


/*fault tolerant SV sweep */
int FaultySVSweep(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                  uint32_t* m_curr, uint32_t* off, uint32_t* ind,
                  double fProb1,         /*probability of bit flip*/
                  double fProb2         /*probability of bit flip for type-2 faults*/
                 )
{
    int changed = 0;

    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            uint32_t u = vind[edge];


            // u = FaultInjectByte(u, 0);
            u = FaultInjectByte(u, fProb1);

            /*sanity check for u*/
            while (u >= nv)    /*a better check can be used*/
            {
                u = vind[edge];

                u = FaultInjectByte(u, fProb1);
            }



            uint32_t cc_prev_u = cc_prev[u];
            cc_prev_u = FaultInjectByte(cc_prev_u, fProb2);

            // cc_prev_u = FaultInjectByte(cc_prev_u, 0);

            if (cc_prev_u < cc_curr[v])
            {
                m_curr[v] = u;
                cc_curr[v] = cc_prev_u;
                changed++;
#ifdef DEBUG
                printf("changed for %d\n", v );
#endif
                if (cc_prev_u != cc_prev[u])
                {
#ifdef DEBUG
                    printf("Error injected for (%d, %d) \n", v, u );
#endif
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


/*fault tolerant SV sweep
Stores the parent in "relative" manner

 */
int FaultySVSweep_RelParent(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                            uint32_t* m_curr, uint32_t* off, uint32_t* ind,
                            double fProb1,         /*probability of bit flip*/
                            double fProb2         /*probability of bit flip for type-2 faults*/
                           )
{
    int changed = 0;

    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];
        MemAccessCount += 5;
        for (size_t edge = 0; edge < vdeg; edge++)
        {
            uint32_t u = vind[edge];
            MemAccessCount++;

            // u = FaultInjectByte(u, 0);
            u = FaultInjectByte(u, fProb1);

            /*sanity check for u*/
            while (u >= nv)    /*a better check can be used*/
            {
                u = vind[edge];
                MemAccessCount++;

                u = FaultInjectByte(u, fProb1);
            }



            uint32_t cc_prev_u = cc_prev[u];
            MemAccessCount++;
            cc_prev_u = FaultInjectByte(cc_prev_u, fProb2);

            // cc_prev_u = FaultInjectByte(cc_prev_u, 0);

            if (cc_prev_u < cc_curr[v])
            {
                m_curr[v] = edge;
                cc_curr[v] = cc_prev_u;
                changed++;
#ifdef DEBUG
                printf("changed for %d\n", v );
#endif
                MemAccessCount++;
#ifdef DEBUG
                if (cc_prev_u != cc_prev[u])
                {

                    printf("Error injected for (%d, %d) \n", v, u );

                    for (int edge = 0; edge < vdeg; ++edge)
                    {

                        uint32_t u = vind[edge];

                    }

                }
#endif

            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}

#define REL_PARENT

uint32_t* FaultTolerantSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
                               stat_t* stat       /*for counting stats of each iteration*/
                             )
{
    /*initialize */
    MemAccessCount = 0;

    /*get fault probability*/
    double fProb1, fProb2;
    // if (getenv("NORM_PROB") != NULL)
    if (0)
    {
        double ind = (double) atof(getenv("NORM_PROB"));
        // int num_edge = off[numVertices];
        fProb1 = pow(2.0, -ind) / ( 32) ;
        fProb2 = fProb1;
        printf("Using fProb1=%g \n", fProb1);
        printf("Using fProb2=%g \n", fProb2);
    }
    else
    {
        if (getenv("FAULT_PROB1") != NULL)
        {
            fProb1 = (double) atof(getenv("FAULT_PROB1"));
            printf("Using fProb1=%g \n", fProb1);

        }
        else
        {
            printf("Environment variable FAULT_PROB1 not set: using default 0\n");
            fProb1 = 0;
        }


        if (getenv("FAULT_PROB2") != NULL)
        {
            fProb2 = (double) atof(getenv("FAULT_PROB2"));
            printf("Using fProb2=%g \n", fProb2);

        }
        else
        {
            printf("Environment variable FAULT_PROB2 not set: using default 0\n");
            fProb2 = 0;
        }
    }

    uint32_t* components_Final = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* components_First = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* modifiers_First = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t** cc_all_iterations = (uint32_t**)malloc(sizeof(uint32_t*)*numVertices);
    uint32_t** m_all_iterations = (uint32_t**)malloc(sizeof(uint32_t*)*numVertices);

    uint32_t* faultOff =  (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* faultInd =  (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* faultEdges = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));



    /* Initialize level array */
    for (size_t i = 0; i < numVertices; i++)
    {
        components_First[i] = i;
#ifdef REL_PARENT
        modifiers_First[i] = -1;
#else
        modifiers_First[i] = i;
#endif

    }

    cc_all_iterations[0] = components_First;
    m_all_iterations[0] = modifiers_First;


    bool changed;
    int num_changes;
    int num_corrections;
    size_t iteration = 0;
    do
    {
        memcpy((void *)faultOff, (void *)off, sizeof(uint32_t)*numVertices );
        memcpy((void *)faultInd, (void *)ind, sizeof(uint32_t)*numEdges);

        uint32_t* cc_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
        uint32_t* m_curr =  (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));

        uint32_t* cc_prev = cc_all_iterations[iteration];
        uint32_t* m_prev  = m_all_iterations[iteration];
        cc_all_iterations[iteration+1] = cc_curr;
        m_all_iterations[iteration+1] = m_curr;

        memcpy((void *)cc_curr, (void *)cc_prev, sizeof(uint32_t)*numVertices );
        memcpy((void *)m_curr, (void *)m_prev, sizeof(uint32_t)*numVertices );


        long long prMemAccessCount = MemAccessCount;
        tic();
        if(iteration==9)
            num_changes = FaultTolerantSVSweep_RelParent(numVertices,
                                              cc_prev, cc_curr, m_curr,
                                              off, ind);

        else
        num_changes = FaultySVSweep_RelParent(numVertices,
                                              cc_prev, cc_curr, m_curr,
                                              off, ind,
                                              fProb1, fProb2);

        stat->SvTime[iteration] = toc();
        stat->SvMemCount[iteration] = MemAccessCount - prMemAccessCount;

        prMemAccessCount = MemAccessCount;
        tic();
        num_corrections = ftBadAdjacencyBadParent_RelParent(numVertices, cc_curr, cc_prev,
                          m_curr, m_prev, off, ind);
        stat->FtTime[iteration] = toc();
        stat->FtMemCount[iteration] = MemAccessCount - prMemAccessCount;


        printf("Executing Iteration     %d: Changes =%d, Corrections=%d\n",
               iteration, num_changes, num_corrections );

        iteration += 1;
    }
    while (num_changes > num_corrections);

    

    stat->numIteration = iteration;
    iteration--;
    memcpy(components_Final, cc_all_iterations[iteration], sizeof(uint32_t)*numVertices );

    for (int i = 0; i < iteration; i++)
    {

        free(cc_all_iterations[i]);
        free(m_all_iterations[i]);
    }

    free(faultEdges);
    free(faultInd);
    free(faultOff);
    free(cc_all_iterations);
    free(m_all_iterations);

    return components_Final;
}
