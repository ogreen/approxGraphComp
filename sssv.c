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





int two_loop_count;
int ss_count;

int lpShortcut_Async(graph_t *graph, lp_state_t*lp_state)
// performs shortcutting and cycle detection togather
{

  size_t nv = graph->numVertices;
  uint32_t*CC = lp_state->CC;
  uint32_t*P = lp_state->P;
  uint32_t*Ps = lp_state->Ps;
  int numChanges = 1;
  int iteration = 0;

  uint32_t* off = graph->off;
  uint32_t* ind = graph->ind;

// now initialize the arrays

  // memory allocation for hj algorithm

  // uint32_t *hjM  = malloc(nv * sizeof(uint32_t));
  // uint32_t *hjD  = malloc(nv * sizeof(uint32_t));
  // uint32_t *hjL  = malloc(nv * sizeof(uint32_t));

  uint32_t *hjM  = lp_state->hjM;
  uint32_t *hjD  = lp_state->hjD;
  uint32_t *hjL  = lp_state->hjL;



  for (uint32_t v = 0; v < nv; v++)
  {
    // hjM[v] = 0;
    // hjD[v] = 0;
    // hjL[v] = -1;
    uint32_t Pv = (Ps[v] == -1) ? v : ind[off[v]  + Ps[v]];
    P[v] = Pv;
    CC[v] = v; // init to parent
  }

  while (numChanges)
  {

    numChanges = 0;
    for (uint32_t v = 0; v < nv; v++)
    {
      if (CC[v] > CC[P[v]] )
      {
        /* code */
        numChanges++;
        CC[v] = CC[P[v]];
      }

    }

    // for (uint32_t v = 0; v < nv; v++)
    // {
      
    //     CC[v] = CC[CC[v]];
      

    // }

  }

  int loop =0;

  for (uint32_t v = 0; v < nv; v++)
  {
    if (CC[v] == v && Ps[v] != -1 )
    {
      /* code */
      loop++;
      Ps[v] = -1;
    }

  }



  return loop;

}

#if 0
int hjShortcut_Async(graph_t *graph, lp_state_t*lp_state)
// performs shortcutting and cycle detection togather
{

  size_t nv = graph->numVertices;
  uint32_t*CC = lp_state->CC;
  uint32_t*P = lp_state->P;
  uint32_t*Ps = lp_state->Ps;
  int numChanges = 1;
  int iteration = 0;

  uint32_t* off = graph->off;
  uint32_t* ind = graph->ind;

// now initialize the arrays

  // memory allocation for hj algorithm

  // uint32_t *hjM  = malloc(nv * sizeof(uint32_t));
  // uint32_t *hjD  = malloc(nv * sizeof(uint32_t));
  // uint32_t *hjL  = malloc(nv * sizeof(uint32_t));

  uint32_t *hjM  = lp_state->hjM;
  uint32_t *hjD  = lp_state->hjD;
  uint32_t *hjL  = lp_state->hjL;



  for (uint32_t v = 0; v < nv; v++)
  {
    hjM[v] = 0;
    hjD[v] = 0;
    hjL[v] = -1;
    uint32_t Pv = (Ps[v] == -1) ? v : ind[off[v]  + Ps[v]];
    P[v] = Pv;
    CC[v] = Pv; // init to parent
  }


  // first shrinkage
  int hjLevel = 0;
  int mark2Del = 1;

  while (mark2Del)
  {
    mark2Del = 0;

    // first mark all the to be deleted nodes
    for (uint32_t v = 0; v < nv; v++)
    {
      // if it is not deleted already
      if (hjD[v] == 0)
      {
        /* code */
        // hjM[v] = (v > P[v]) && (P[v] <= P[P[v]] );
        hjM[v] = (v > P[v]) ;
        mark2Del +=  hjM[v];
      }

    }

    // now delete all the marked nodes
    for (uint32_t v = 0; v < nv; v++)
    {
      // if it is not deleted already
      if (hjD[v] == 0)
      {

        // and if my parent is marked to deleted then I jump
        while (hjM[P[v]])
        {
          P[v] = P[P[v]];      // the jump operation
        }

        if (hjM[v])
        {
          hjL[v] = hjLevel;  //delete level
          hjD[v] = 1;    // marked as deleted
        }

        

      }
    }
    hjLevel++;

    // printf("Number of vertex marked for deletion: %d \n", mark2Del );

  }

  // printf("Number of iteration %d\n",  hjLevel);
  int loop = 0;
  // check if there is any loop
  for (uint32_t v = 0; v < nv; v++)
  {
    // if it is not deleted already
    if (hjD[v] == 0)
    {
      // if I am not my parent
      if (Ps[v] != -1)
      {
        // P[P[v]] = v;   // reverse the direction
        Ps[v] = -1;
        loop++;
        CC[v] = v;
      }
    }
  }



#if 0
  // it should ideally never come here
  // now reverse the linked list
  for (uint32_t v = 0; v < nv; v++)
  {
    // if it is not deleted already
    if (hjD[v] == 0)
    {
      // if I am not my parent
      if (P[v] != v)
      {
        P[P[v]] = v;   // reverse the direction
      }
    }
  }


  int loop = 0;
  // now remove the two loop
  for (uint32_t v = 0; v < nv; v++)
  {
    // if it is not deleted already
    if (hjD[v] == 0)
    {
      // if I am not my parent
      if (P[P[v]] == v && P[v] > v )
      {
        P[v] = v;   // reverse the direction

      }
    }
  }

  // now shortcut the loop

  numChanges = 1;
  while (numChanges)
  {
    numChanges = 0;
    for (uint32_t v = 0; v < nv; v++)
    {
      // if it is not deleted already
      if (hjD[v] == 0)
      {
        // if it is not converged

        if (P[P[v]] != P[v])
        {
          P[v] = P[P[v]];
          numChanges++;
        }

      }
    }
  }

#endif
  // now go through level by level to broadcast the root
  for (int lvl = hjLevel - 1; lvl > -1 ; --lvl)
  {
    /* code */
    for (uint32_t v = 0; v < nv; v++)
    {
      // if it is not deleted already
      if (hjL[v] == lvl)
      {

        CC[v] = CC[P[v]];
      }
    }

  }


  // free(hjL);
  // free(hjD);
  // free(hjM);

  return loop;

}

#endif 

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
    if (v == 2)
    {
      //printf("// v=%d, Ps=%d, Pv=%d\n", v, lp_state->Ps[v], Pv);
      // printf("Check0: %d  %d %d\n", v, CC[v], P[v] );
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
        // printf("Check1: %d  %d %d\n", v, CC[v], P[v] );
      }
      uint32_t tmp = MIN(CC[v], CC[P[v]]);
      if (CC[v] != tmp)
      {
        if (v == 3615)
        {
          // printf("Check2: %d  %d %d\n", v, CC[v], P[v] );
        }
        numChanges++;


        CC[v] = MIN(CC[v], CC[P[v]]);


      }

    }

    for (uint32_t v = 0; v < nv; v++)
    {
      P[v] =  P[P[v]];
    }

  }

  // printf("Number of iteration %d\n", iteration );

  // loop detect and correct
  int loop = 0;
  int uroots = 0;
  uint32_t* off = graph->off;
  uint32_t* ind = graph->ind;
  for (uint32_t v = 0; v < nv; v++)
  {
    CC[v] = MIN(v, CC[v]);
    if (v == CC[v])
    {
      if (lp_state->Ps[v] != -1 )
      {
        // printf("//Loop found %d\n", v );
        loop++;
        lp_state->Ps[v] = -1;
        // const uint32_t *restrict vind = &ind[off[v]];
        // const size_t vdeg = off[v + 1] - off[v];

        // for (size_t edge = 0; edge < vdeg; edge++)
        // {
        //     const uint32_t u = vind[edge];
        //     if (lp_state->CC[u] < lp_state->CC[v])
        //     {
        //         lp_state->Ps[v] = edge;
        //         lp_state->CC[v] = lp_state->CC[u];
        //         uroots++;
        //     }
        // }
      }


    }

  }

  // printf("finish self-stabilizing Shortcutting %d\n", iteration);
  // return uroots;
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


  int loop = 0;
  for (uint32_t v = 0; v < nv; v++)
  {
    CC[v] = MIN(v, CC_prev[v]);
    if (v == CC[v])
    {
      if (lps_curr->Ps[v] != -1 )
      {

        loop++;
        lps_curr->Ps[v] = -1;

      }


    }

  }

  return loop;



}





int SSstep_Async(graph_t *graph, lp_state_t*lp_state)
// detects cycle in the linked list and resolve the cycle issues
/*makes the arbitrary step lp_state of label propagation algorithm
self-stabilizing; i.e.
lp_state<-SS(lp_state, graph)
LP(lp_state) -> correct solution
*/
{

  printParentTree("Before_stabilization_step", graph, lp_state);

  size_t nv = graph->numVertices;
  uint32_t*CC = lp_state->CC;
  /*first check following condition for all vertex

  1.  P[v] \in N(v) => Ps[v]+2>=1 & Ps[v] < |adj(v)|
  2.  CC[v] >= CC[P[v]]
  */
  uint32_t v = 3615;
  if (v == 3615)
  {
    // printf("Check0: %d  %d \n", v, CC[v] );
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
        // printf("Check1: %d  %d \n", v, CC[v] );
      }

      corrupted++;

    }

    else if ((lp_state->Ps[v] >= vdeg && lp_state->Ps[v] != -1) || lp_state->Ps[v] + 2 < 1)
    {

      if (v == 3615)
      {
        // printf("Check2: %d  %d \n", v, CC[v] );
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
          // printf("Check3: %d  %d \n", v, CC[v] );
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
            // printf("Check4: %d  %d \n", v, CC[v] );
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

  printParentTree("After_first_checks", graph, lp_state);

  if (v == 3615)
  {
    // printf("Check5: %d  %d \n", v, CC[v] );
  }
  /*Now do the Cycle Correction*/



  int loops = 0;
  loops = ssShortcut_Async(graph, lp_state);

  // loops = hjShortcut_Async(graph, lp_state);
  // loops = lpShortcut_Async(graph, lp_state);
  // printf("//Number of corruptions is %d, number of loop %d, NV %d\n", corrupted, loops, nv );
  corrupted += loops;

  if (v == 3615)
  {
    // printf("Check0: %d  %d \n", v, CC[v] );
  }

  printParentTree("After_short_cut", graph, lp_state);

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



  int loop = 0;

  loop = ssShortcut_Sync(graph, lp_state_out, lp_state_in);
  // printf("//NUmber of corruptions is %d, loops %d, NV %d\n", corrupted,loop, nv );
  corrupted += loop;

  return corrupted;
}


static inline uint32_t FaultInjectWord(uint32_t value)
{

  uint32_t mask = 0;

  mask += (1 << rand() % 32);
  return mask ^ (value);

}

static long long MemAccessCount;

/*fault tolerant SV Async sweep */
int  FISVSweep_Async(graph_t *graph, lp_state_t *lp_state,
                     uint32_t* FaultArrEdge,         /*probability of bit flip*/
                     uint32_t* FaultArrCC )        /*probability of bit flip for type-2 faults*/
{
  size_t nv = graph->numVertices;
  uint32_t* off = graph->off;
  uint32_t* ind = graph->ind;
  uint32_t* component_map = lp_state->CC;
  uint32_t* CC = lp_state->CC;

  int  changed = 0;

  // 2-loop detection heuristic
  for (size_t v = 0; v < nv; v++)
  {

    uint32_t* vind = &ind[off[v]];
    if (lp_state->Ps[v] == -1)
    {
      /* code */
      CC[v] = v;
    }
    else
    {
      int Pv = vind[lp_state->Ps[v]];
      if (CC[v] < CC[Pv])
      {
        /* code */
        CC[v] = v;
        lp_state->Ps[v] = -1;
      }
      else
      {
        int PPv;
        if (lp_state->Ps[Pv] == -1)
        {
          /* code */
          PPv = Pv;
        }
        else
        {
          PPv = ind[off[Pv] + lp_state->Ps[Pv]];
          if (v == PPv)
          {
            /* code */
            CC[v] = v;
            lp_state->Ps[v] == -1;
          }
        }


      }
    }
  }

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

/*fault tolerant SV Sync sweep */
int  FIHSVSweep_Sync(graph_t *graph,
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

  // 2-loop detection heuristic
  int tmp_2_loop = two_loop_count;
  for (size_t v = 0; v < nv; v++)
    // if(0)
  {
    // break;

    uint32_t* vind = &ind[off[v]];
    if (lp_state_in->Ps[v] == -1)
    {
      /* code */
      CC[v] = v;
    }
    else
    {
      int Pv = vind[lp_state_in->Ps[v]];
      if (CC[v] < CC[Pv])
      {
        /* code */
        CC[v] = v;
        lp_state_in->Ps[v] = -1;
      }
      else
      {
        int PPv;
        if (lp_state_in->Ps[Pv] == -1)
        {
          /* code */
          PPv = Pv;
        }
        else
        {
          PPv = ind[off[Pv] + lp_state_in->Ps[Pv]];
          if (v == PPv)
          {
            /* code */
            CC[v] = v;
            lp_state_in->Ps[v] = -1;
            two_loop_count++;
          }
        }


      }
    }
    lp_state_out->CC[v] = lp_state_in->CC[v];
    lp_state_out->Ps[v] = lp_state_in->Ps[v];
  }

  // if (two_loop_count > tmp_2_loop)
  // {
  //     ss_count++;
  //     SSstep_Sync(graph, lp_state_in, lp_state_out);
  // }

  printParentTree("loop_2_detect", graph, lp_state_in);

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
          // printf("// Error injected Edge %d-->%d\n", uT, u );
        }

        /* code */
        //printf("//v=6  u=%d  uT=%d\n", u, uT );

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
          if ( var < cc_prev_u)

          {
            // printf("// Error injected CC %d-->%d\n", CC[u], var );
          }
          // do
          // {
          //     var = FaultInjectWord(cc_prev_u);
          //     // printf("stuck 2 %u %u\n", var, u);
          // }
          // while (var > u);

          /* code */
          //printf("//v=%d  CC[u]=%d  CC[u]T=%d\n",v, cc_prev_u, var );

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
        if (v == 129)
        {
          /* code */
          // printf("// %d-->%d--->", lp_state_out->CC[v], cc_prev_u );
        }
        uint32_t* uind = &ind[off[u]];
        uint32_t Pu = lp_state_in->Ps[u] == -1 ? u : uind[lp_state_in->Ps[u]];
        // if (v != Pu)
        if (1)
        {
          lp_state_out->Ps[v] = edge;
          lp_state_out->CC[v] = cc_prev_u;
          changed = 1;
        }
      }
    }
    // if (v == 129)
    // {printf("\n");}
  }

  /*shortcutting goes here*/
  return changed;

}

/*fault tolerant SV Sync sweep */
int  FIHSVSweep_Async(graph_t *graph,
                      lp_state_t* lp_state_cur,      // Current LP State
                      uint32_t* FaultArrEdge,       /*probability of bit flip*/
                      uint32_t* FaultArrCC )        /*probability of bit flip for type-2 faults*/
{
  size_t nv = graph->numVertices;
  uint32_t* off = graph->off;
  uint32_t* ind = graph->ind;
  uint32_t* CC = lp_state_cur->CC;

  int  changed = 0;

  // 2-loop detection heuristic
  int tmp_2_loop = two_loop_count;
  for (size_t v = 0; v < nv; v++)
    // if(0)
  {
    // break;

    uint32_t* vind = &ind[off[v]];
    if (lp_state_cur->Ps[v] == -1)
    {
      /* code */
      CC[v] = v;
    }
    else
    {
      int Pv = vind[lp_state_cur->Ps[v]];
      if (CC[v] < CC[Pv])
      {
        /* code */
        CC[v] = v;
        lp_state_cur->Ps[v] = -1;
      }
      else
      {
        int PPv;
        if (lp_state_cur->Ps[Pv] == -1)
        {
          /* code */
          PPv = Pv;
        }
        else
        {
          PPv = ind[off[Pv] + lp_state_cur->Ps[Pv]];
          if (v == PPv)
          {
            /* code */
            CC[v] = v;
            lp_state_cur->Ps[v] = -1;
            two_loop_count++;
          }
        }


      }
    }
  }

  // if (two_loop_count > tmp_2_loop)
  // {
  //     ss_count++;
  //     SSstep_Sync(graph, lp_state_in, lp_state_out);
  // }

  printParentTree("loop_2_detect", graph, lp_state_cur);

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
          // printf("// Error injected Edge %d-->%d\n", uT, u );
        }

        /* code */
        //printf("//v=6  u=%d  uT=%d\n", u, uT );

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
          if ( var < cc_prev_u)

          {
            // printf("// Error injected CC %d-->%d\n", CC[u], var );
          }
          // do
          // {
          //     var = FaultInjectWord(cc_prev_u);
          //     // printf("stuck 2 %u %u\n", var, u);
          // }
          // while (var > u);

          /* code */
          //printf("//v=%d  CC[u]=%d  CC[u]T=%d\n",v, cc_prev_u, var );

        }
        else
        {
          var = cc_prev_u;
        }

        cc_prev_u = var;
      }

      // Write to the auxiliary copy
      if (cc_prev_u < CC[v])
      {
        uint32_t* uind = &ind[off[u]];
        uint32_t Pu = lp_state_cur->Ps[u] == -1 ? u : uind[lp_state_cur->Ps[u]];
        lp_state_cur->Ps[v] = edge;
        CC[v] = cc_prev_u;
        changed = 1;
      }
    }
  }

  /*shortcutting goes here*/
  return changed;

}

int SSHSVAlg_Async( lp_state_t *lp_state,  graph_t *graph,
                    stat_t* stat, int ssf // frequency of self stabilization
                    , int max_iter
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
    changed = FIHSVSweep_Async(graph, lp_state, FaultArrEdge, FaultArrCC) ;
    iteration += 1;
    char label[100];
    sprintf(label, "Iteration_%d", iteration);
    printParentTree(label, graph, lp_state);
    if (iteration % ssf == 0 || !changed)
    {
      // if (!changed) printf("//convergence detected %d\n", iteration );
      corrupted = SSstep_Async(graph, lp_state);
    }

    // printf("//Finished iteration  %d\n", iteration );
  }
  while ((changed || corrupted) && iteration < max_iter);

  /*updating stats*/
  stat->numIteration = iteration;
  free (FaultArrEdge);
  free (FaultArrCC);
  // printf("// Number of iteration is %d\n", iteration );
  if (iteration == max_iter)
  {
    /* code */
    return 1;
  }

  return 0;
}

int SSSVAlg_Async( lp_state_t *lp_state,  graph_t *graph,
                   stat_t* stat, int ssf // frequency of self stabilization
                   , int max_iter
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
    char label[100];
    sprintf(label, "Iteration_%d", iteration);
    printParentTree(label, graph, lp_state);
    if (iteration % ssf == 0 || !changed)
    {
      // if (!changed) printf("//convergence detected %d\n", iteration );
      corrupted = SSstep_Async(graph, lp_state);
    }

    // printf("//Finished iteration  %d\n", iteration );
  }
  while ((changed || corrupted) && iteration < max_iter);

  /*updating stats*/
  stat->numIteration = iteration;
  free (FaultArrEdge);
  free (FaultArrCC);
  // printf("// Number of iteration is %d\n", iteration );
  if (iteration == max_iter)
  {
    /* code */
    return 1;
  }

  return 0;
}

int SSSVAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                  stat_t* stat, int ssf // frequency of self stabilization
                  , int max_iter
                )
{
  two_loop_count = 0;
  ss_count = 0;
  size_t numVertices  = graph->numVertices;
  size_t numEdges  = graph->numEdges;
  uint32_t* off  = graph->off;
  uint32_t* ind  = graph->ind;

  // Allocate auxillary state
  lp_state_t lp_state_aux;
  alloc_lp_state(graph, &lp_state_aux);
  init_lp_state(graph, &lp_state_aux);
  lp_state_t* lp_state_cur = &lp_state_aux;

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
    // printf("//Two loop count is %d\n", two_loop_count);
    // printf("//ss count is %d\n", ss_count);
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
    char label[100];
    sprintf(label, "Iteration_%d", iteration);
    printParentTree(label, graph, lp_state_prev);
    // changed = FIHSVSweep_Sync(graph, lp_state_prev, lp_state_cur, FaultArrEdge, FaultArrCC) ;
    changed =  FISVSweep_Sync(numVertices,
                              lp_state_prev->CC, lp_state_cur->CC, lp_state_cur->Ps,
                              off, ind,
                              FaultArrEdge, FaultArrCC);

    iteration += 1;
    sprintf(label, "Iteration_%d_after", iteration - 1);
    printParentTree(label, graph, lp_state_cur);


    // printParentTree(label, graph, lp_state_prev);
    if (iteration % ssf == 0 || !changed)
    {
      // if (!changed) printf("//convergence detected %d\n", iteration );
      corrupted = SSstep_Sync(graph, lp_state_cur, lp_state_prev);
      // printf
    }

    // Copy over current state to previous
    memcpy(lp_state_prev->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));
    memcpy(lp_state_prev->Ps, lp_state_cur->Ps, numVertices * sizeof(uint32_t));

    // printf("//Finished iteration  %d\n", iteration );
  }
  while ((changed || corrupted) && iteration < max_iter);

  /*updating stats*/
  stat->numIteration = iteration;
  free (FaultArrEdge);
  free (FaultArrCC);
  // printf("// Number of iteration is %d\n", iteration );

  if (iteration == max_iter)
  {
    /* code */
    return 1;
  }
  return 0;
}


/*with heuristic*/
int SSHSVAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                   stat_t* stat, int ssf // frequency of self stabilization
                   , int max_iter
                 )
{
  two_loop_count = 0;
  ss_count = 0;
  size_t numVertices  = graph->numVertices;
  size_t numEdges  = graph->numEdges;
  uint32_t* off  = graph->off;
  uint32_t* ind  = graph->ind;

  // Allocate auxillary state
  lp_state_t lp_state_aux;
  alloc_lp_state(graph, &lp_state_aux);
  init_lp_state(graph, &lp_state_aux);
  lp_state_t* lp_state_cur = &lp_state_aux;

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
    // printf("//Two loop count is %d\n", two_loop_count);
    // printf("//ss count is %d\n", ss_count);
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
    char label[100];
    sprintf(label, "Iteration_%d", iteration);
    printParentTree(label, graph, lp_state_prev);
    changed = FIHSVSweep_Sync(graph, lp_state_prev, lp_state_cur, FaultArrEdge, FaultArrCC) ;
    // changed =  FISVSweep_Sync(numVertices,
    //                           lp_state_prev->CC, lp_state_cur->CC, lp_state_cur->Ps,
    //                           off, ind,
    //                           FaultArrEdge, FaultArrCC);

    iteration += 1;
    sprintf(label, "Iteration_%d_after", iteration - 1);
    printParentTree(label, graph, lp_state_cur);


    // printParentTree(label, graph, lp_state_prev);
    if (iteration % ssf == 0 || !changed)
    {
      // if (!changed) printf("//convergence detected %d\n", iteration );
      corrupted = SSstep_Sync(graph, lp_state_cur, lp_state_prev);
      // printf
    }

    // Copy over current state to previous
    memcpy(lp_state_prev->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));
    memcpy(lp_state_prev->Ps, lp_state_cur->Ps, numVertices * sizeof(uint32_t));

    // printf("//Finished iteration  %d\n", iteration );
  }
  while ((changed || corrupted) && iteration < max_iter);

  /*updating stats*/
  stat->numIteration = iteration;
  free (FaultArrEdge);
  free (FaultArrCC);
  // printf("// Number of iteration is %d\n", iteration );

  if (iteration == max_iter)
  {
    /* code */
    return 1;
  }
  return 0;
}


int FISVAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                  stat_t* stat, int ssf // frequency of self stabilization
                  , int max_iter
                )
// fault injected label propagation algorithm;
// will give incorrect output (most likely)
// used to measure failure rate
{
  size_t numVertices  = graph->numVertices;
  size_t numEdges  = graph->numEdges;
  uint32_t* off  = graph->off;
  uint32_t* ind  = graph->ind;

  // Allocate auxillary state
  lp_state_t lp_state_aux;
  alloc_lp_state(graph, &lp_state_aux);
  init_lp_state(graph, &lp_state_aux);
  lp_state_t* lp_state_cur = &lp_state_aux;

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
    char label[100];
    sprintf(label, "Iteration_%d", iteration);
    printParentTree(label, graph, lp_state_prev);
    // changed = FIHSVSweep_Sync(graph, lp_state_prev, lp_state_cur, FaultArrEdge, FaultArrCC) ;
    changed =  FISVSweep_Sync(numVertices,
                              lp_state_prev->CC, lp_state_cur->CC, lp_state_cur->Ps,
                              off, ind,
                              FaultArrEdge, FaultArrCC);

    iteration += 1;
    sprintf(label, "Iteration_%d_after", iteration);
    printParentTree(label, graph, lp_state_prev);




    // Copy over current state to previous
    memcpy(lp_state_prev->CC, lp_state_cur->CC, numVertices * sizeof(uint32_t));
    memcpy(lp_state_prev->Ps, lp_state_cur->Ps, numVertices * sizeof(uint32_t));

    // printf("//Finished iteration  %d\n", iteration );
  }
  while ((changed || corrupted) && iteration < max_iter);

  /*updating stats*/
  stat->numIteration = iteration;
  free (FaultArrEdge);
  free (FaultArrCC);
  // printf("// Number of iteration is %d\n", iteration );

  return 0;
}

int FISVAlg_Async( lp_state_t* lp_state_prev, graph_t *graph,
                   stat_t* stat, int ssf // frequency of self stabilization
                   , int max_iter
                 )
// fault injected label propagation algorithm;
// will give incorrect output (most likely)
// used to measure failure rate
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
    char label[100];
    sprintf(label, "Iteration_%d", iteration);
    printParentTree(label, graph, lp_state_prev);
    // changed = FIHSVSweep_Sync(graph, lp_state_prev, lp_state_cur, FaultArrEdge, FaultArrCC) ;
    changed =  FISVSweep_Async(graph, lp_state_prev, FaultArrEdge, FaultArrCC);

    iteration += 1;
    sprintf(label, "Iteration_%d_after", iteration);
    printParentTree(label, graph, lp_state_prev);

    // printf("//Finished iteration  %d\n", iteration );
  }
  while ((changed || corrupted) && iteration < max_iter);

  /*updating stats*/
  stat->numIteration = iteration;
  free (FaultArrEdge);
  free (FaultArrCC);
  // printf("// Number of iteration is %d\n", iteration );

  return 0;
}

