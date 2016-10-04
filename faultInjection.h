#pragma once 

#include <stdint.h>

uint32_t FaultInjectByte(uint32_t value, double prob);

const char *byte_to_binary(uint32_t x);
int flip(double prob);

int getFault_prob(double *fProb1, double *fProb2);