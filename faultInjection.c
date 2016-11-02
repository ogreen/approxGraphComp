#include <stdio.h>      /* printf */
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */
#include <stdbool.h>
#include <stdint.h>

#include "faultInjection.h"


bool bitset(void const * data, int bitindex)
{
    int byte = bitindex / 8;
    int bit = bitindex % 8;
    unsigned char const * u = (unsigned char const *) data;
    return (u[byte] & (1 << bit)) != 0;
}


const char *byte_to_binary(uint32_t x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

const char *uint32_to_binary(uint32_t x)
{
    static char b[33];
    b[32] = '\0';
    for (int i = 0; i < 32; ++i)
    {
        b[i] = '0';

    }
    // printf("the string b %s\n",b );

    int plc = 1;

    while (x > 0)
    {
        if (x % 2 == 1)
        {
            b[32 - plc] = '1';

        }


        plc++;
        x = x >> 1;


    }

    return b;
}



static inline uint32_t FaultInjectBit(uint32_t value, double prob)
{
    uint32_t mask = 0;

    for (int i = 0; i < 8 * sizeof(uint32_t); ++i)
    {
        if (flip(prob))
        {
            mask += (1 << i);
            // printf("filpped %d\n",i );
        }
    }

    // printf("The value           %s\n", uint32_to_binary(*value) );
    // printf("The mask            %s\n", uint32_to_binary(mask) );
    // printf("The masked value    %s\n", uint32_to_binary(mask^(*value)) );

    /*uses xor to flip the positions where mask is set*/
    return mask ^ (value);
}

static inline uint32_t FaultInjectWord(uint32_t value, double prob)
{
    double random_num = (double) rand() / ((double) RAND_MAX + 1.0);
    if (random_num > 32 * prob)
    {
        return value;
    }
    else
    {


        uint32_t mask = 0;

        mask += (1 << rand()%32);
        return mask ^ (value);
    }
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


uint32_t FaultInjectByte(uint32_t value, double prob)
{
    // return FaultInjectBit(value, prob);
    return FaultInjectWord(value, prob);
}


int getFault_prob(double *fProb1, double *fProb2)
{
    if (getenv("NORM_PROB") != NULL)
        // if (0)
    {
        double ind = (double) atof(getenv("NORM_PROB"));
        // int num_edge = off[numVertices];
        // fProb1 = pow(2.0, -ind) / ( 32) ;
        *fProb1 = pow(2.0, -ind) ;
        *fProb2 = *fProb1;
#ifdef DEBUG
        printf("Using *fProb1=%g \n", *fProb1);
        printf("Using *fProb2=%g \n", *fProb2);
#endif
    }
    else
    {
        if (getenv("FAULT_PROB1") != NULL)
        {
            *fProb1 = (double) atof(getenv("FAULT_PROB1"));
#ifdef DEBUG
            printf("Using *fProb1=%g \n", *fProb1);
#endif
        }
        else
        {
            printf("Environment variable FAULT_PROB1 not set: using default 0\n");
            *fProb1 = 0;
        }


        if (getenv("FAULT_PROB2") != NULL)
        {
            *fProb2 = (double) atof(getenv("FAULT_PROB2"));
#ifdef DEBUG
            printf("Using *fProb2=%g \n", *fProb2);
#endif

        }
        else
        {
            printf("Environment variable FAULT_PROB2 not set: using default 0\n");
            *fProb2 = 0;
        }
    }

}

