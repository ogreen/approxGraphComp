#pragma once


#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "stat.h"

uint32_t* BaselineSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
	 stat_t* stat);

uint32_t* FaultTolerantSVMain( size_t numVertices, size_t numEdges, 
	uint32_t* off, uint32_t* ind);

uint32_t* FaultFreeSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind,
	 stat_t* stat);