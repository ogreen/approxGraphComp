#include "main.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include "sv.h"


#define COUNTOF(array) (sizeof(array) / sizeof(array[0]))

void readGraphDIMACS(char* filePath, uint32_t** prmoff, uint32_t** prmind, uint32_t* prmnv, uint32_t* prmne)
{
    FILE *fp = fopen (filePath, "r");
    int32_t nv, ne;

    char* line = NULL;

    // Read data from file
    int32_t temp, lineRead;
    size_t bytesRead = 0;
    getline (&line, &bytesRead, fp);


    sscanf (line, "%d %d", &nv, &ne);


    free(line);
    int32_t * off = (int32_t *) malloc ((nv + 2) * sizeof (int32_t));
    int32_t * ind = (int32_t *) malloc ((ne * 2) * sizeof (int32_t));
    off[0] = 0;
    off[1] = 0;
    int32_t counter = 0;
    int32_t u;
    line = NULL;
    bytesRead = 0;


    for (u = 1; (temp = getline (&line, &bytesRead, fp)) != -1; u++)
    {
        uint32_t neigh = 0;
        uint32_t v = 0;
        char *ptr = line;
        int read = 0;
        char tempStr[1000];
        lineRead = 0;
        while (lineRead < bytesRead && (read = sscanf (ptr, "%s", tempStr)) > 0)
        {
            v = atoi(tempStr);
            read = strlen(tempStr);
            ptr += read + 1;
            lineRead = read + 1;
            neigh++;
            ind[counter++] = v;
        }
        off[u + 1] = off[u] + neigh;
        free(line);
        bytesRead = 0;
    }


    fclose (fp);


    nv++;
    ne *= 2;
    *prmnv = nv;
    *prmne = ne;
    *prmind = ind;
    *prmoff = off;
}

int main (const int argc, char *argv[])
{

    uint32_t nv, ne, naction;
    uint32_t* off;
    uint32_t* ind;
    if (argc < 2)
    {
        printf("usage: %s GraphFile\n", argv[0]);
        exit(1);

    }
    readGraphDIMACS(argv[1], &off, &ind, &nv, &ne);

    uint32_t* cc_baseline = BaselineSVMain( nv, ne, off, ind);
    uint32_t* cc_ft = FaultTolerantSVMain( nv, ne, off, ind);

    for (int i = 0; i < nv; ++i)
    {
        /* code */
          assert(cc_ft[i]==cc_baseline[i]);

    }
    printf("Output correct!\n");

    free(cc_ft);
    free(cc_baseline);


    free(off);
    free(ind);
    return 0;
}



// void Benchmark_ConnectedComponents_SV( 
//      ConnectedComponents_SV_Function sv_function, 
//     size_t numVertices, size_t numEdges, uint32_t* off, uint32_t* ind)
// {
    

//     uint32_t* components_map = (uint32_t*)memalign(64, numVertices * sizeof(uint32_t));



//     /* Initialize level array */
//     for (size_t i = 0; i < numVertices; i++)
//     {
//         components_map[i] = i;
//     }

//     bool changed;
//     size_t iteration = 0;
//     do
//     {

//         //printf("@@ %d @@ ",algPerIteration[iteration]);
//         changed = (sv_function)(numVertices, components_map, off, ind);

//         iteration += 1;
//     }
//     while (changed);

//     free(components_map);
// }





