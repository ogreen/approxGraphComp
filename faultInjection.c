#include <stdio.h>      /* printf */
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */
#include <stdbool.h>
#include <stdint.h>

#include "faultInjection.h"


bool bitset(void const * data, int bitindex) {
  int byte = bitindex / 8;
  int bit = bitindex % 8;
  unsigned char const * u = (unsigned char const *) data;
  return (u[byte] & (1<<bit)) != 0;
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

    int plc =1;

    while(x>0)
    {
        if (x%2==1)
        {
            b[32-plc] = '1';
            
        }
        

        plc++;
        x =x>>1;
        

    }

    return b;
}



uint32_t FaultInjectByte(uint32_t value, double prob)
{
    uint32_t mask =0; 

    for (int i = 0; i < 8*sizeof(uint32_t); ++i)
    {
        if (flip(prob))
        {
            mask += (1<<i);
            // printf("filpped %d\n",i );
        }
    }

    // printf("The value           %s\n", uint32_to_binary(*value) );
    // printf("The mask            %s\n", uint32_to_binary(mask) );
    // printf("The masked value    %s\n", uint32_to_binary(mask^(*value)) );

    /*uses xor to flip the positions where mask is set*/
    return mask^(value); 
}

int flip(double prob)
{
    double random_num = (double) rand()/((double) RAND_MAX+1.0);
    if (random_num<prob)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}


void FaultInjection(void* arr, size_t sz)
/*flips bits random*/
{


}

