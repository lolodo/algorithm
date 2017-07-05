/*
题目：输入一个已经按升序排序过的数组和一个数字，
在数组中查找两个数，使得它们的和正好是输入的那个数字。
要求时间复杂度是O(n)。如果有多对数字的和等于输入的数字，输出任意一对即可。
例如输入数组1、2、4、7、11、15 和数字15。由于4+11=15，因此输出4 和11。
*/

#include <stdio.h>
#include <stdlib.h>

int array[] = {1, 2, 4, 7, 11, 15};

void find2num(int a[], int length, int number)
{
    int i, j;

    i = 0;
    j = length - 1;

    while(i < j) {
        if (a[i] + a[j] > number) {
            j--;
        } else if(a[i] + a[j] < number) {
            i++;
        } else {
            printf("the %dth:%d, the %dth:%d\n", i, a[i], j, a[j]);
            return;
        }
    }

    printf("No result!\n");
}

int main(int argc, char *args[])
{
    int input;
    int len;

    printf("Please input the number:");
    scanf("%d", &input);

    len = sizeof(array)/sizeof(int);
    find2num(array, len, input);

    return 0;
}
