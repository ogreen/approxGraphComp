#ifndef _FAULTINJECTION_H_
#define _FAULTINJECTION_H_

#include <stdint.h>



const char *byte_to_binary(uint32_t x);
int flip(double prob);

int getFault_prob(double *fProb1, double *fProb2);
uint32_t FaultInjectByte(uint32_t value, double prob);
#endif 