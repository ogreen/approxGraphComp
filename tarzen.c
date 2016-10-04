


// This file contains implementation for tarzen's algorithm for strongly connected component in the graph
#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include "sv.h"

// #define MIN(A, B) ((A < B) ? A : B)

/** the algorithm from wikipedia
 algorithm tarjan is
  input: graph G = (V, E)
  output: set of strongly connected components (sets of vertices)

  index := 0
  S := empty
  for each v in V do
    if (v.index is undefined) then
      strongconnect(v)
    end if
  end for

  function strongconnect(v)
    // Set the depth index for v to the smallest unused index
    v.index := index
    v.lowlink := index
    index := index + 1
    S.push(v)
    v.onStack := true

    // Consider successors of v
    for each (v, w) in E do
      if (w.index is undefined) then
        // Successor w has not yet been visited; recurse on it
        strongconnect(w)
        v.lowlink  := min(v.lowlink, w.lowlink)
      else if (w.onStack) then
        // Successor w is in stack S and hence in the current SCC
        v.lowlink  := min(v.lowlink, w.index)
      end if
    end for

    // If v is a root node, pop the stack and generate an SCC
    if (v.lowlink = v.index) then
      start a new strongly connected component
      repeat
        w := S.pop()
        w.onStack := false
        add w to current strongly connected component
      while (w != v)
      output the current strongly connected component
    end if
  end function

  **/


#define MIN(a,b) a>b?b:a

typedef  std::stack<int> mystack;


typedef struct
{

	mystack S;
	int global_index;
	int* vind;
	int* vlowlink;
	int* vonstack;

} ts_t;	// tarzen structure





void init_ts(int nv, ts_t *ts)
{
	ts->vind = (int*) malloc(sizeof(int) * nv);
	ts->vlowlink = (int*) malloc(sizeof(int) * nv);
	ts->vonstack = (int*) malloc(sizeof(int) * nv);

	for (int i = 0; i < nv; ++i)
	{
		ts->vind[i] = -1;
		ts->vlowlink[i] = 0;
		ts->vonstack[i] = 0;
	}
}

void free_ts(ts_t *ts)
{
	free(ts->vind );
	free(ts->vlowlink );
	free(ts->vonstack );
}






int strongConnected(int ii, graph_t *graph, lp_state_t  *lp_state,
                    ts_t *ts)
{
	int* vind = ts->vind;
	int* vlowlink = ts->vlowlink;
	int* vonstack = ts->vonstack;
	vind[ii] = ts->global_index;
	vlowlink[ii] = ts->global_index;
	ts->global_index = ts->global_index + 1;
	(ts->S).push(ii);
	vonstack[ii] = 1;



	if (!(lp_state->Ps[ii] == -1))
		// it must not be self-loop
	{
		/* code */
		// int jj = P[ii];
		// int jj = P[ii];
		int jj = graph->ind[graph->off[ii]  + lp_state->Ps[ii]] ;


		if (vind[jj] == -1)
		{
			// strongConnected(jj, S, global_index, nv, CC, P, vind, vlowlink, vonstack);
			strongConnected(jj, graph, lp_state, ts);
			// strongConnected(i, S, global_index, nv, CC, P, vind, vlowlink, vonstack);
			vlowlink[ii] = MIN(vlowlink[ii], vlowlink[jj]);
		}
		else if (vonstack[jj] == 1)
		{
			/* code */
			vlowlink[ii] = MIN(vlowlink[ii], vlowlink[jj]);
		}
	}



	if (vind[ii] == vlowlink[ii])
	{

		/* code */
		int mn = ii;  /*min label*/
		int jj;



		printf("The SCC ");
		do
		{
			jj = (ts->S).top();
			mn = MIN( mn , jj );
			(ts->S).pop();

			printf("%d  ->", jj );
		}
		while (jj != ii);

		printf(": minimum elements is %d\n", mn);

		/*correction step */
		lp_state->Ps[mn] = -1;
		lp_state->CC[mn] = mn;
	}
}


/* Shortcutting */
int shortcut_LP(graph_t *graph, lp_state_t*lp_state, ts_t *ts)
/*
for each vertex in the graph it changes the label of the vertex to the
root of the tree
*/
{
	size_t nv = graph->numVertices;
	uint32_t*CC = lp_state->CC;
	int numChanges = 1;
	while (numChanges > 0)
	{
		numChanges = 0;
		for (uint32_t v = 0; v < nv; v++)
		{
			/* code */
			uint32_t Pv = lp_state->Ps[v] == -1 ? v : graph->ind[graph->off[v]  + lp_state->Ps[v]];
			// printf("v=%u Pv=%u ",v, Pv);
			uint32_t PPv =  lp_state->Ps[Pv] == -1 ? Pv : graph->ind[graph->off[Pv]  + lp_state->Ps[Pv]];
			// printf("PPv=%u\n", PPv );
			if (CC[v] != CC[PPv])
			{
				CC[v] = CC[ PPv];
				numChanges++;
			}
		}

		// printf("finish iteration\n");
	}

}


int selfStab_LP(graph_t *graph, lp_state_t*lp_state, ts_t *ts)
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
	for (uint32_t v = 0; v < nv; v++)
	{
		const size_t vdeg = graph->off[v + 1] - graph->off[v];

		if (lp_state->Ps[v] >= vdeg || lp_state->Ps[v] + 2 < 1)
		{
			/* reset that node */
			lp_state->Ps[v] = -1;
			lp_state->CC[v] = v;
		}
		else
		{
			uint32_t Pv = graph->ind[graph->off[v]  + lp_state->Ps[v]];
			if (lp_state->CC[v] < lp_state->CC[Pv])
			{
				/* reset that node */
				lp_state->Ps[v] = -1;
				lp_state->CC[v] = v;
			}
		}


	}

	/*Now do the Cycle Correction*/


	ts->global_index = 0;
	for (int i = 0; i < graph->numVertices; ++i)
	{
		ts->vind[i] = -1;
		ts->vlowlink[i] = 0;
		ts->vonstack[i] = 0;
	}

	for (int i = 0; i < graph->numVertices; ++i)
	{
		if (ts->vind[i] == -1)
		{
			/* node not yet discovered */
			strongConnected(i, graph, lp_state, ts);

		}
	}

	/*finally use short cut to set all the nodes */
	shortcut_LP(graph, lp_state, ts);

	return 0;
}




int cycleDetect(graph_t *graph, lp_state_t*lp_state, ts_t *ts)
// detects cycle in the linked list and resolve the cycle issues
{

	ts->global_index = 0;
	for (int i = 0; i < graph->numVertices; ++i)
	{
		ts->vind[i] = -1;
		ts->vlowlink[i] = 0;
		ts->vonstack[i] = 0;
	}

	for (int i = 0; i < graph->numVertices; ++i)
	{
		if (ts->vind[i] == -1)
		{
			/* node not yet discovered */
			strongConnected(i, graph, lp_state, ts);

		}
	}

}


int main(int argc, char const *argv[])
{

	graph_t graph;
	graph.numVertices = 12;
	graph.numEdges = 22;

	uint32_t off[13] = {0, 2, 4, 6, 7, 10, 12, 14, 16, 17, 18, 20, 22};
	uint32_t ind[22] = {10, 11,
	                    2, 3,
	                    1, 4,
	                    1,
	                    2, 8, 9,
	                    6, 7,
	                    5, 7,
	                    5, 6,
	                    4,
	                    4,
	                    0, 11,
	                    0, 10
	                   };

	graph.off = off;
	graph.ind = ind;


	uint32_t CC[12];

	for (int i = 0; i < 12; ++i)
	{
		CC[i] = i;
	}
	uint32_t P[12] = {11, 1, 1, 1, 2, 7, 5, 6, 4, 4, 0, 10};

	// uint32_t P[12] = {11, 1, 1, 1, 2, 7, 5, 6, 4, 4, 0, 10};
	// uint32_t Ps[12] = {1, -1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1};
	uint32_t Ps[12] = {1, -1, 0, 0, 0, -1, 0, 1, 0, 0, 0, 1};

	lp_state_t lp_state;

	lp_state.CC = CC;
	lp_state.Ps = Ps;




	ts_t ts;
	// ts = (ts_t *) malloc(sizeof(ts_t));
	init_ts(12, &ts);

	// cycleDetect(&graph, &lp_state, &ts);

	// shortcut_LP(&graph, &lp_state, &ts);

	selfStab_LP(&graph, &lp_state, &ts);
	printf("After Self-stabilizing\n");

	// cycleDetect(&graph, &lp_state, &ts);

	for (int i = 0; i < 12; ++i)
	{
		printf("%u \n", lp_state.CC[i]);
	}
	// cycleDetect(12, CC, P, &ts);
	free_ts(&ts);

	return 0;
}