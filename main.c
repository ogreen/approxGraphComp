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
#include "stat.h"

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

    /*starting the stat*/

    stat_t statBL, statFF;
    InitStat(&statBL);
    uint32_t* cc_baseline = BaselineSVMain( nv, ne, off, ind, &statBL);
    uint32_t* cc_ff = FaultFreeSVMain( nv, ne, off, ind, &statFF);
    for (int i = 0; i < nv; ++i)
    {
        /* code */
        assert(cc_ff[i] == cc_baseline[i]);

    }
    printf("Output correct!\n");



    uint32_t* cc_ft = FaultTolerantSVMain( nv, ne, off, ind);

    for (int i = 0; i < nv; ++i)
    {
        /* code */
        assert(cc_ft[i] == cc_baseline[i]);

    }
    printf("Output correct!\n");
    PrintStat(&statBL);

    free(cc_ft);
    free(cc_baseline);


    free(off);
    free(ind);
    return 0;
}






