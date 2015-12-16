
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>

bool BaselineSVSweep(size_t nv, uint32_t* component_map, uint32_t* off, uint32_t* ind) {
  bool changed = false;

  for (size_t v=0; v < nv; v++) {
	const uint32_t *restrict vind = &ind[off[v]];
	const size_t vdeg = off[v + 1] - off[v];

	for (size_t edge = 0; edge < vdeg; edge++){
	  const uint32_t u = vind[edge];
	  if (component_map[u] < component_map[v]) {
		component_map[v] = component_map[u];
		changed = true;
	  }
	}
  }

  /*shortcutting goes here*/
  return changed;

}



uint32_t* BaselineSVMain( size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind)
{
    

    uint32_t* components_map = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));
	/* Initialize level array */
    for (size_t i = 0; i < numVertices; i++)
    {
        components_map[i] = i;
    }

    bool changed;
    size_t iteration = 0;
    do
    {
		changed = BaselineSVSweep(numVertices, components_map, off, ind);
		iteration += 1;
    }
    while (changed);

    return components_map;
}


