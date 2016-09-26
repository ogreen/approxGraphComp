


// This file contains implementation for tarzen's algorithm for strongly connected component in the graph
#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


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






// int strongConnected(int ii, mystack& S, int &global_index, int nv, uint32_t* CC, uint32_t* P,
//                     int* vind, int* vlowlink, int* vonstack)
int strongConnected(int ii, int nv, uint32_t* CC, uint32_t* P,
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

	if (!(P[ii] == ii))
		// it must not be self-loop
	{
		/* code */
		int jj = P[ii];
		if (vind[jj] == -1)
		{
			// strongConnected(jj, S, global_index, nv, CC, P, vind, vlowlink, vonstack);
			strongConnected(jj, nv, CC, P, ts);
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
		int jj;
		printf("The SCC ");
		do
		{
			jj = (ts->S).top();
			(ts->S).pop();

			printf("%d  ->", jj );
		}
		while (jj != ii);

		printf("\n");
	}
}

int cycleDetect(int nv, uint32_t* CC, uint32_t* P,
                ts_t *ts)
// detects cycle in the linked list and resolve the cycle issues
{
	// mystack S;
	// ts->S = S;
	ts->global_index = 0;
	for (int i = 0; i < nv; ++i)
	{
		ts->vind[i] = -1;
		ts->vlowlink[i] = 0;
		ts->vonstack[i] = 0;
	}



	for (int i = 0; i < nv; ++i)
	{
		if (ts->vind[i] == -1)
		{
			/* node not yet discovered */
			strongConnected(i,  nv, CC, P, ts);

		}
	}

}


int main(int argc, char const *argv[])
{


	uint32_t CC[12];
	uint32_t P[12] = {11, 1, 1, 1, 2, 7, 5, 6, 4, 4, 0, 10};
	int ind[12];
	int lowlink[12];
	int vonstack[12];

	ts_t ts; 
	// ts = (ts_t *) malloc(sizeof(ts_t));
	init_ts(12, &ts);


	cycleDetect(12, CC, P, &ts);
	free_ts(&ts);

	return 0;
}