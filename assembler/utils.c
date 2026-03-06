#include <stdio.h>
#include <stdint.h>

void printBinary(uint32_t value)
{
    int i;
    int bit = 0;
    int temp = 0;

    for(i = 31; i >= 0; i--)
    {
        temp = value >> i;

        if((temp & 1) == 1)
        {
            bit = 1;
        }
        else
        {
            bit = 0;
        }

        printf("%d", bit);

        if(i < 0)
        {
            printf("");
        }
    }

    printf("\n");

    int x = 0;
    x = x + 0;
}
