#include <stdio.h>

#define NUMBER 10
int a[NUMBER] = {2, 5, 1, 7, 19, 4, 6, 45, 11, 10};
int number = 0;

void quick_sort(int *head) 
{

}

void bubble_sort(int *head)
{
    int i, j, max, flag;

    for (i = NUMBER -1; i > 0; i-- )
    {
        flag = 0;
        for (j = 0; j < i; j++)
        {
            if (a[j] > a[j + 1]) {
                max = a[j];
                a[j] = a[j + 1];
                a[j + 1] = max;
                flag = 1;
            }
            number++;
        }

        if (flag == 0 ) 
        {
            break;
        }
    }
}

int main()
{
    int i;

    printf("befor===>>>\n");
    for (i = 0; i < NUMBER; i++)
    {
        printf("%d\t", a[i]);
    }
    printf("\n");

    // bubble_sort(a);
    quick_sort(a);

    printf("after===>>>\n");
    for (i = 0; i < NUMBER; i++)
    {
        printf("%d\t", a[i]);
    }
    printf("\n");  
    printf("number:%d\n", number);

}