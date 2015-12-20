
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>

#include "sv.h"
#include "faultInjection.h"

/*checks for BadAdjacency type faults and if possible corrects it*/
void ftBadAdjacency(size_t nv,
 uint32_t* cc_curr, uint32_t* cc_prev,
 uint32_t* m_curr, uint32_t* m_prev, 
 uint32_t* off, uint32_t* ind)
{

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
            printf("Error detected - BadAdjacency %d.... correcting\n", v);
            cc_curr[v] = cc_prev[v];
            m_curr[v] = m_prev[v];
        }
    }
}

/*detect for Bad parent type faults*/
void ftBadParent(size_t nv,  uint32_t* m_curr, uint32_t* m_prev, uint32_t* off, uint32_t* ind)
{
    for (size_t v = 0; v < nv; v++)
    {
        if (m_curr[v] == v )
            continue;
        if (m_curr[m_prev[v]] == v )
            printf("Error detected: Bad Parent\n");
    }
}


/*fault tolerant SV sweep */
bool FaultTolerantSVSweep(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                          uint32_t* m_curr, uint32_t* off, uint32_t* ind)
{
    bool changed = false;

    for (size_t v = 0; v < nv; v++)
    {
        // if (v == 2)
        // {
        //     m_curr[2] = 3;
        //     cc_curr[2] = 0;
        //     continue;
        // }
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            const uint32_t u = vind[edge];


//            if(faultEdges[off[v]+edge] )
//                  FAULTY EDGE
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
bool FaultySVSweep(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                   uint32_t* m_curr, uint32_t* off, uint32_t* ind,
                   double fProb         /*probability of bit flip*/
                  )
{
    bool changed = false;

    for (size_t v = 0; v < nv; v++)
    {
        // if (v == 2)
        // {
        //     m_curr[2] = 3;
        //     cc_curr[2] = 0;
        //     continue;
        // }
        const uint32_t *restrict vind = &ind[off[v]];
        const size_t vdeg = off[v + 1] - off[v];

        for (size_t edge = 0; edge < vdeg; edge++)
        {
            // const uint32_t u = vind[edge];
            // injecting the fault  in reading adjacency list
            uint32_t u = vind[edge];

            u = FaultInjectByte(u, fProb);

            /*sanity check for u*/
            while (u >= nv)    /*a better check can be used*/
            {
                u = vind[edge];

                u = FaultInjectByte(u, fProb);
            }



            uint32_t cc_prev_u = cc_prev[u];
            // cc_prev_u = FaultInjectByte(cc_prev_u, fProb);
            cc_prev_u = FaultInjectByte(cc_prev_u, 0);

            if (cc_prev_u < cc_curr[v])
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




uint32_t* FaultTolerantSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind)
{

    printf("checking masking ....\n");
    uint32_t frc = 23478239;
    FaultInjectByte(frc, 0.125);
    FaultInjectByte(frc, 0.125);

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
        modifiers_First[i] = i;

    }

    cc_all_iterations[0] = components_First;
    m_all_iterations[0] = modifiers_First;


    bool changed;
    size_t iteration = 1;
    do
    {
        memcpy((void *)faultOff, (void *)off, sizeof(uint32_t)*numVertices );
        memcpy((void *)faultInd, (void *)ind, sizeof(uint32_t)*numEdges);

        uint32_t* cc_curr = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
        uint32_t* m_curr =  (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));

        uint32_t* cc_prev = cc_all_iterations[iteration - 1];
        uint32_t* m_prev  = m_all_iterations[iteration - 1];
        cc_all_iterations[iteration] = cc_curr;
        m_all_iterations[iteration] = m_curr;

        memcpy((void *)cc_curr, (void *)cc_prev, sizeof(uint32_t)*numVertices );
        memcpy((void *)m_curr, (void *)m_prev, sizeof(uint32_t)*numVertices );

        // Error injection
        // Mark the faulty in array faultEdge[e]


        // changed = FaultTolerantSVSweep(numVertices, cc_prev, cc_curr, m_curr, off, ind);
        changed = FaultySVSweep(numVertices, cc_prev, cc_curr, m_curr, off, ind, 1e-5);

        // Detection and correction
        ftBadAdjacency(numVertices, cc_curr, cc_prev, m_curr, m_prev, off, ind);
        ftBadParent(numVertices,  m_curr, m_prev, off, ind);

        iteration += 1;
    }
    while (changed);

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
