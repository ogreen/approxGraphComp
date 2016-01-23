#include <stdio.h>


#include "stat.h"

int InitStat(stat_t* stat)
{
	for (int i = 0; i < MAX_SV_SWEEP; ++i)
	{
		/* code */
		stat->SvTime[i] = 0; 		/*time for each SvItertation*/
		stat->FtTime[i] = 0; 		/*time for each fault detection and correction*/
		stat->SvMemCount[i] = 0; 		/*#memory access for each SvItertation*/
		stat->FtMemCount[i] = 0;
	}

	return 0;
}


int PrintStat(stat_t* stat)
{
	for (int i = 0; i < stat->numIteration; ++i)
	{
		printf("%lld\t %lld\t %lld\t %lld\t\n",
			stat->SvTime[i],stat->FtTime[i],stat->SvMemCount[i], stat->FtMemCount[i]);
	}

	return 0;
}