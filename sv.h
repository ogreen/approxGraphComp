#pragma once


#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "stat.h"

uint32_t* BaselineSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
	 stat_t* stat);

uint32_t* FaultTolerantSVMain( size_t numVertices, size_t numEdges, 
	uint32_t* off, uint32_t* ind,stat_t* stat, int max_iter);

uint32_t* FaultFreeSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
	 stat_t* stat);

/*for checking the overhead*/
uint32_t* FTSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
                           stat_t* stat       /*for counting stats of each iteration*/
                         , int max_iter);