#pragma once 

#include <inttypes.h>
#include <asm/unistd.h>


typedef struct
{
	size_t numVertices;
	size_t numEdges;
	uint32_t *off;
	uint32_t *ind;

} graph_t;


void readGraphDIMACS(char* filePath, graph_t *graph);

char *ExtractGraphName(char* s);

void free_graph(graph_t* graph);