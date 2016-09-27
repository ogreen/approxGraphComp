#pragma once


#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "stat.h"

typedef struct
{

	uint32_t* CC; // connected component array
	uint32_t* Ps; // parent array in index format

} lp_state_t;	// label propagation state


typedef struct
{
	size_t numVertices;
	size_t numEdges;
	uint32_t *off;
	uint32_t *ind;

} graph_t;



// uint32_t* BaselineSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
// 	 stat_t* stat);

// uint32_t* FaultTolerantSVMain( size_t numVertices, size_t numEdges,
// 	uint32_t* off, uint32_t* ind, stat_t* stat, int max_iter);

// uint32_t* FaultFreeSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
// 	 stat_t* stat);

// /*for checking the overhead*/
// uint32_t* FTSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
//                            stat_t* stat       /*for counting stats of each iteration*/
//                          , int max_iter);



uint32_t* BaselineSVMain( graph_t *graph,
                          stat_t* stat);

uint32_t* FaultTolerantSVMain( graph_t *graph,
                               stat_t* stat, int max_iter);

uint32_t* FaultFreeSVMain( graph_t *graph,
                           stat_t* stat);

/*for checking the overhead*/
uint32_t* FTSVMain( graph_t *graph,
                    stat_t* stat       /*for counting stats of each iteration*/
                    , int max_iter);