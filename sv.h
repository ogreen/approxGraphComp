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

} lp_state_t;	// label propagation state



int FFSVAlg_Async(lp_state_t *lp_state, graph_t *graph, stat_t* stat);

lp_state_t SCSVAlg_Sync( graph_t *graph, stat_t* stat, int max_iter);

lp_state_t FFWoSVAlg_Sync( graph_t *graph, stat_t* stat);

/*for checking the overhead*/
lp_state_t FFSVAlg_Sync( graph_t *graph, stat_t* stat, int max_iter);


/*allocates space for a lp_state*/
int alloc_lp_state(graph_t *graph, lp_state_t *lp_state);

/*frees space for a lp_state*/
int free_lp_state(lp_state_t *lp_state);

/*initializes an lp_state*/
int init_lp_state(graph_t *graph, lp_state_t *lp_state);