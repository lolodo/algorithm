#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char a[10]={1,2,3,4,5,6,7,8,9,10};
    int size;

    size = sizeof(a);

    printf("size is %d\n", size);
    
    return 0;
}