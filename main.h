#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef bool (*ConnectedComponents_SV_Function)(size_t vertexCount, uint32_t* componentMap, uint32_t* vertexEdges, uint32_t* neighbors);

bool ConnectedComponents_SV_Branchy_PeachPy(size_t vertexCount, uint32_t* componentMap, uint32_t* vertexEdges, uint32_t* neighbors);
bool ConnectedComponents_SV_Branchless_PeachPy(size_t vertexCount, uint32_t* componentMap, uint32_t* vertexEdges, uint32_t* neighbors);


