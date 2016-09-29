
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

#include "graph.h"

// graph.numVertices=nv;
    // graph.numEdges=ne;
    // graph.off=off;
    // graph.ind=ind;


void readGraphDIMACS(char* filePath, graph_t *graph)
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

    // printf("nv =%u, ne=%u\n",nv,ne );
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
    // *prmnv = nv;
    // *prmne = ne;
    // *prmind = ind;
    // *prmoff = off;
    graph->numVertices=nv;
    graph->numEdges=ne;
    graph->off=off;
    graph->ind=ind;

}

/*extracts name of the matrix from file given from path*/
char *ExtractGraphName(char* s)
{

    const char dlm[2] = "/.";
    char *GraphName =  strdup(s);
    char *GraphName1 =  strtok(GraphName, dlm);
    char *GraphName2 =  strtok(NULL, dlm);
    char *GraphName3 =  strtok(NULL, dlm);

    if (GraphName1 != NULL && GraphName2 != NULL)
    {
        /* code */
        while (GraphName3 != NULL)
        {
            GraphName1 = GraphName2;
            GraphName2 = GraphName3;
            GraphName3 =  strtok(NULL, dlm);

        }
    }
    else
    {
        printf("Error occured\n");
    }

#ifdef DEBUG
    printf("extension free file name is %s\n", GraphName1 );
#endif
    return GraphName1;

}


void free_graph(graph_t* graph)
{
    free(graph->off);
    free(graph->ind);
}