
#pragma once
// file contain description and routines for collecting execution stat stats 

#define MAX_SV_SWEEP 1000 /*maximum number of sv sweep allowed*/

typedef long long ltime; 	/*to use rtdsc timer*/ 
typedef long long lcount; 	/*to count number of memory access*/ 

typedef struct 
{
	/* data */
	int numIteration;					/*number of iteration to converge*/
	double SvTime[MAX_SV_SWEEP]; 		/*time for each SvItertation*/
	double FtTime[MAX_SV_SWEEP]; 		/*time for each fault detection and correction*/
	lcount SvMemCount[MAX_SV_SWEEP]; 		/*#memory access for each SvItertation*/
	lcount FtMemCount[MAX_SV_SWEEP]; 		/*#memory access for each fault detection and correction*/
	lcount NumChanges[MAX_SV_SWEEP]; 		/*#memory access for each fault detection and correction*/
	lcount NumCorrection[MAX_SV_SWEEP]; 		/*#memory access for each fault detection and correction*/

} stat_t;

int InitStat(stat_t* stat);


int PrintStat(stat_t* stat);


/*prints the status of two stat baseline and fault tolerant: to compare them and export them*/
int PrintCompStat(stat_t* stat_BL, stat_t* stat_FT);


int PrintCompStat2(char* GraphName, int norm_prob, 
	stat_t* stat_BL,stat_t* stat_FF, stat_t* stat_FT);