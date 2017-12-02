#pragma once


#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "stat.h"
#include "graph.h"
typedef struct
{

	uint32_t* CC; // connected component array
	uint32_t* Ps; // parent array in index format
	uint32_t* P; // parent array in absolute format
	int* Cr;

     uint32_t *hjM ;
   uint32_t *hjD ;
   uint32_t *hjL ;


} lp_state_t;	// label propagation state



int FFSVAlg_Async(lp_state_t *lp_state, graph_t *graph, stat_t* stat);

lp_state_t SCSVAlg_Sync( graph_t *graph, stat_t* stat, int max_iter);

lp_state_t FFWoSVAlg_Sync( graph_t *graph, stat_t* stat);

/*for checking the overhead*/
lp_state_t FFSVAlg_Sync( graph_t *graph,
                     lp_state_t lps_curr,
                     stat_t* stat,       /*for counting stats of each iteration*/
                     int max_iter        /*contgrolling maximum number of iteration*/
                   );


/*allocates space for a lp_state*/
int alloc_lp_state(graph_t *graph, lp_state_t *lp_state);

/*frees space for a lp_state*/
int free_lp_state(lp_state_t *lp_state);

/*initializes an lp_state*/
int init_lp_state(graph_t *graph, lp_state_t *lp_state);

int printParentTree(char *name, graph_t* graph, lp_state_t *lp_state);
int printGraph(char *name, graph_t* graph, lp_state_t *lp_state);


/*test-1: Random Init 
Starts with random starting state; 
runs label propagation algorithm, checks the output with baseline; 
*/
int rand_lp_state( graph_t *graph,
                               lp_state_t *lp_state);


/*test-2
Random-flip output test;
Run a correct LP algorithm and randomly flip some of the output at the end.
*/


int rand_flip_output( double fprob,             // probability of flipping an entry
    graph_t *graph,
                               lp_state_t *lp_state);