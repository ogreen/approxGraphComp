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
		printf("%g\t %g\t %lld\t %lld\t\n",
			stat->SvTime[i],stat->FtTime[i],stat->SvMemCount[i], stat->FtMemCount[i]);
	}

	return 0;
}

/*prints the status of two stat baseline and fault tolerant: to compare them and export them*/
int PrintCompStat(stat_t* stat_BL, stat_t* stat_FT)
{
	
	for (int i = 0; i < stat_FT->numIteration; ++i)
	{
		printf("%g, %g, %g, %lld, %lld, %lld\n",
			stat_BL->SvTime[i],stat_FT->SvTime[i],stat_FT->FtTime[i],
			stat_BL->SvMemCount[i],stat_FT->SvMemCount[i], stat_BL->FtMemCount[i]);
	}

	return 0;
}


/*prints the status of two stat baseline and fault tolerant: to compare them and export them*/
int PrintCompStat2(stat_t* stat_BL,stat_t* stat_FF, stat_t* stat_FT)
{
	
	for (int i = 0; i < stat_FT->numIteration; ++i)
	{
		printf("%e, %e, %e, %e, %lld, %lld, %lld\n",
			stat_BL->SvTime[i],stat_FF->SvTime[i],stat_FT->SvTime[i],stat_FT->FtTime[i],
			stat_BL->SvMemCount[i],stat_FT->SvMemCount[i], stat_FT->FtMemCount[i]);
	}

	return 0;
}