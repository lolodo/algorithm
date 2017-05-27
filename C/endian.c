#include <stdio.h>

union endian_judge {
    unsigned int value;
    unsigned char endian;
};

int main(void)
{
    union endian_judge test;

    test.value = 0x80000000;

    if (test.endian) {
        printf("big endian!\n");
    } else {
        printf("little endian!\n");
    }
    
    return 0;
}