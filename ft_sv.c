
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

off_t fsize(const char *filename)
{
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));

    return -1;
}




uint32_t* FaultArr;
uint32_t FaultArrPtr;
uint32_t FaultArrSz;



uint32_t FaultInjectByteFile(uint32_t val)
{
    return FaultArr[(FaultArrPtr++) % FaultArrSz] ^ val;

}

// #define FAULT_INJECT_FILE

// uncomment following for debug statements
// #define DEBUG

// the variable to keep count of number of memory accesses
static long long MemAccessCount;



/*checks for BadAdjacency type faults and if possible
corrects it*/
int ftBadAdjacencyBadParent_RelParent(size_t nv,
                                      uint32_t* cc_curr, uint32_t* cc_prev,
                                      uint32_t* m_curr, uint32_t* m_prev,
                                      uint32_t* off, uint32_t* ind)
{
    int corrections = 0;

    /*now correcting bad parent*/
// #pragma omp parallel for 
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
#ifdef DEBUG
            printf("2.Error detected - BadParent %d.... correcting\n", v);
#endif
            // if(v==6085 || v==6084)printf("correcting for v=%d, cc[v] =%d\n",v,cc_curr[v] );
            /*do the correction*/
            corrections++;
            for (size_t edge = 0; edge < vdeg; edge++)
            {
                const uint32_t u = vind[edge];
                MemAccessCount += 2;
                // if(v==6085) printf(" u=%d cc_prev[u]=%d\n",u,cc_prev[u]);
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
                // if(v==6085) printf(" u=%d cc_prev[u]=%d\n",u,cc_prev[u]);
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
// #pragma omp parallel for 
    for (size_t v = 0; v < nv; v++)
    {

        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];
        MemAccessCount += 5;
        for (size_t edge = 0; edge < vdeg; edge++)
        {
            uint32_t u = vind[edge];
            MemAccessCount++;
            // if(v==270) printf("%d ->",u  );

            // u = FaultInjectByte(u, 0);
#ifdef FAULT_INJECT_FILE
            u = FaultInjectByteFile(u);
#else
            u = FaultInjectByte(u, fProb1);
#endif
            /*sanity check for u*/
            while (u >= nv)    /*a better check can be used*/
            {
                u = vind[edge];
                MemAccessCount++;

#ifdef FAULT_INJECT_FILE
                u = FaultInjectByteFile(u);
#else
                u = FaultInjectByte(u, fProb1);
#endif
            }
            // if(v==6085) printf("%d ->",u  );
            if (u != vind[edge])
            {
                // printf("Bad adjacency injected at (%d %d -> %d)\n",v,vind[edge],u );
            }

            uint32_t cc_prev_u = cc_prev[u];
            uint32_t var;
            MemAccessCount++;
            do
            {
#ifdef FAULT_INJECT_FILE
                var = FaultInjectByteFile(cc_prev_u);
#else
                var = FaultInjectByte(cc_prev_u, fProb2);
#endif
            }
            while (var > u);
            cc_prev_u = var;

            // if(v==6085) printf("%d \n",cc_prev_u );
            // cc_prev_u = FaultInjectByte(cc_prev_u, 0);

            if (cc_prev_u < cc_curr[v])
            {
                m_curr[v] = edge;
                cc_curr[v] = cc_prev_u;
                changed++;
// #ifdef DEBUG
                // if(v==6085) printf("changed for %d, cc[%d]=%d\n, edge =%d", v,v,cc_curr[v],edge );
// #endif
                MemAccessCount++;
// #ifdef DEBUG
                if (cc_prev_u != cc_prev[u])
                {

                    // if(v==6085)           printf("Error injected for (%d, %d) \n", v, u );

                    for (int edge = 0; edge < vdeg; ++edge)
                    {

                        uint32_t u = vind[edge];

                    }

                }
// #endif

            }
        }
    }


    /*shortcutting goes here*/
    return changed;

}


uint32_t FaultInjectWord(uint32_t value)
{

    uint32_t mask = 0;

    mask += (1 << rand() % 32);
    return mask ^ (value);

}


int FaultySVSweep_FaultArr(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
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

#define REL_PARENT




uint32_t* FaultTolerantSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
                               stat_t* stat,       /*for counting stats of each iteration*/
                               int max_iter        /*contgrolling maximum number of iteration*/
                             )
{
    /*initialize */

    MemAccessCount = 0;

    /*get fault probability*/
    double fProb1, fProb2;


    if (getenv("NORM_PROB") != NULL)
        // if (0)
    {
        double ind = (double) atof(getenv("NORM_PROB"));
        // int num_edge = off[numVertices];
        // fProb1 = pow(2.0, -ind) / ( 32) ;
        fProb1 = pow(2.0, -ind) ;
        fProb2 = fProb1;
#ifdef DEBUG
        printf("Using fProb1=%g \n", fProb1);
        printf("Using fProb2=%g \n", fProb2);
#endif
    }
    else
    {
        if (getenv("FAULT_PROB1") != NULL)
        {
            fProb1 = (double) atof(getenv("FAULT_PROB1"));
#ifdef DEBUG
            printf("Using fProb1=%g \n", fProb1);
#endif
        }
        else
        {
            printf("Environment variable FAULT_PROB1 not set: using default 0\n");
            fProb1 = 0;
        }


        if (getenv("FAULT_PROB2") != NULL)
        {
            fProb2 = (double) atof(getenv("FAULT_PROB2"));
#ifdef DEBUG
            printf("Using fProb2=%g \n", fProb2);
#endif

        }
        else
        {
            printf("Environment variable FAULT_PROB2 not set: using default 0\n");
            fProb2 = 0;
        }
    }

    uint32_t* cc_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* cc_prev = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* m_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
    uint32_t* m_prev = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));


    uint32_t* FaultArrEdge = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));
    uint32_t* FaultArrCC = (uint32_t*)memalign(64, numEdges * sizeof(uint32_t));

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

        // for (int i = 0; i < numEdges; ++i)
        // {

        //     double random_num = (double) rand() / ((double) RAND_MAX + 1.0);
        //     if (random_num < fProb1)
        //     {
        //         /* code */
        //         FaultArrEdge[i] = 1;
        //     }
        //     else
        //     {
        //         FaultArrEdge[i] = 0;
        //     }

        //     random_num = (double) rand() / ((double) RAND_MAX + 1.0);

        //     if (random_num < fProb2)
        //     {
        //         /* code */
        //         FaultArrCC[i] = 1;
        //     }
        //     else
        //     {
        //         FaultArrCC[i] = 0;
        //     }


        // }

        for (int i = 0; i < numEdges; ++i)
        {
            /* code */
            FaultArrEdge[i] = 0;
            FaultArrCC[i] = 0;
        }

        int numEdgeFault = 0;
        int numCCFault = 0;

        while (numEdgeFault <  0.5*fProb1 * numEdges)
        {
            uint32_t ind = rand() % numEdges;
            FaultArrEdge[ind] = 1;
            numEdgeFault++;

        }

        while (numCCFault <  0.5*fProb2 * numEdges)
        {
            uint32_t ind = rand() % numEdges;
            FaultArrCC[ind] = 1;
            numCCFault++;

        }




        if (0)
            num_changes = FaultTolerantSVSweep_RelParent(numVertices,
                          cc_prev, cc_curr, m_curr,
                          off, ind);

        else
        {
#if 1
            num_changes = FaultySVSweep_FaultArr(numVertices,
                                                 cc_prev, cc_curr, m_curr,
                                                 off, ind,
                                                 FaultArrEdge, FaultArrCC);
#else
            num_changes = FaultySVSweep_RelParent(numVertices,
                                                  cc_prev, cc_curr, m_curr,
                                                  off, ind,
                                                  fProb1, fProb2);
#endif
        }
        // 0.0, 0.0);

        stat->SvTime[iteration] = toc();
        stat->SvMemCount[iteration] = MemAccessCount - prMemAccessCount;

        prMemAccessCount = MemAccessCount;
        tic();
        num_corrections = ftBadAdjacencyBadParent_RelParent(numVertices, cc_curr, cc_prev,
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
    }
    while (num_changes > num_corrections && iteration <= max_iter);
    stat->numIteration = iteration;
#ifdef DEBUG
    printf("NUmber of iteration for fault free=%d\n", iteration );
#endif
    free(cc_prev);
    free (FaultArr);

    free (FaultArrEdge);
    free (FaultArrCC);
    return cc_curr;
}