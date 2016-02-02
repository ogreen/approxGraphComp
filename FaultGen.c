#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>


uint32_t FaultInjectByte(double prob);
int flip(double prob);
int RandomGenCheck(void);
#include <time.h>

static struct timespec tic_ts;

void tic(void)
{
	clock_gettime(CLOCK_MONOTONIC, &tic_ts);
}

double toc(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((double)ts.tv_nsec - (double)tic_ts.tv_nsec) * 1.0e-9 + ((double)ts.tv_sec - (double)tic_ts.tv_sec);
}



int main(int argc, char const *argv[])
{


	if (argc < 5)
	{
		printf("usage: %s #num_edges #num_iteration #norm_prob #outputfile\n", argv[0]);
		exit(1);

	}

	uint32_t numEdge = atoi(argv[1]);
	uint32_t numIter = atoi(argv[2]);
	uint32_t norm_prob = atoi(argv[3]);

	printf("using numEdge=%d, numIter=%d, norm_prob=%d, %d\n", numEdge, numIter, norm_prob, numEdge * numIter );
	double fProb = pow(2.0, (double) - norm_prob) / ( 32) ;

	uint32_t* faultArr = (uint32_t* ) malloc(sizeof(uint32_t) * numEdge * numIter);

	tic();
	// #pragma omp parallel
	{
		// int tid = omp_get_thread_num();
		// srand(tid);
		// #pragma omp for schedule(static)
		for (uint32_t i = 0; i < numEdge * numIter; ++i)
		{
			faultArr[i] = FaultInjectByte(fProb);
		}
	}
	double tgen = toc();
	printf("Time generating array=%g\n", tgen );


	/*now writing the file in binary format*/
	FILE* fp;
	fp = fopen(argv[4], "w");
	tic();
	fwrite(faultArr, sizeof(uint32_t), numEdge * numIter, fp );
	tgen = toc();
	printf("Time writing array=%g\n", tgen );

// RandomGenCheck();

	fclose(fp);
	free(faultArr);

	return 0;


}


uint32_t FaultInjectByte(double prob)
{
	uint32_t mask = 0;
	

	for (int i = 0; i < 8 * sizeof(uint32_t); ++i)
	{
		if (flip(prob))
		{
			mask += (1 << i);

		}
	}


	return mask;
}

int flip(double prob)
{
	double random_num = (double) rand() / ((double) RAND_MAX + 1.0);
	if (random_num < prob)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}


int RandomGenCheck(void)
{
	uint32_t RngArr[64];
	for (int i = 0; i < 64; ++i)
	{
		RngArr[i] = 0;
	}

	#pragma omp parallel
	{
		int tid = omp_get_thread_num();
		srand(tid);
		printf("starting  from thread =%d\n", tid );
		#pragma omp for
		for (int i = 0; i < 64; ++i)
		{
			RngArr[i] = rand();
		}

	}

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			printf("%u ", RngArr[j + 8 * i] );
		}
		printf("\n" );
	}

	return 0;
}
