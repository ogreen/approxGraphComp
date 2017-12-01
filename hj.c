// Helman-JaJa algorithm implementation from jaja

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "listutils.h"

void parallelListRanks (long head, const long* next, long* rank, size_t n) {
  int L = (int) sqrt(n);

  //Handling small list case
  if (L + 1 > n)
    L = 1;

  long *V = (long*) malloc(L * sizeof(long));
  long *startOfSeg = (long*) malloc(L * sizeof(long));
  long *nextSeg =  (long*) malloc(L * sizeof(long));

  //Using rank to mark boundaries of the segments.
  #pragma omp parallel for
  for (long i = 0; i < n; i++)
    rank[i] = NIL;

  //Randomly choosing start of segments.
  rank[head] = 0;
  startOfSeg[0] = head;
  for (long i = 1; i < L; i++) {
    long j;

    do {
      j = (int)(n * rand() / (RAND_MAX + 1.0));
    } while (next[j] == NIL || rank[j] != NIL);

    rank[j] = i;
    startOfSeg[i] = j;
  }

  //Computing the length of each segment (in V) and its next segment.
  #pragma omp parallel for
  for (long i = 0; i < L; i++) {
    long j = startOfSeg[i];

    V[i] = 0;
    do {
      V[i]++;
      j = next[j];
    } while (j != NIL && rank[j] == NIL);

    if (j == NIL)
      nextSeg[i] = NIL;
    else
      nextSeg[i] = rank[j];
  }

  //Computing the starting rank value for each segment.
  long cur_seg = 0;
  long start_from = 0;
  do {
    start_from += V[cur_seg];
    V[cur_seg] = start_from - V[cur_seg];
    cur_seg = nextSeg[cur_seg];
  } while (cur_seg != NIL);

  //Walking along each segment to set the rank.
  #pragma omp parallel for
  for (long i = 0; i < L; i++) {
    long j = startOfSeg[i];
    long r = V[i];

    do {
      rank[j] = r++;
      j = next[j];
    } while (j != NIL && rank[j] == NIL);
  }

  free(V);
  free(startOfSeg);
  free(nextSeg);
}