/*
3.求子数组的最大和
题目：
输入一个整形数组，数组里有正数也有负数。
数组中连续的一个或多个整数组成一个子数组，每个子数组都有一个和。
求所有子数组的和的最大值。要求时间复杂度为O(n)。
例如输入的数组为1, -2, 3, 10, -4, 7, 2, -5，和最大的子数组为3, 10, -4, 7, 2，
因此输出为该子数组的和18。
ANSWER: 
A traditional greedy approach.
Keep current sum, slide from left to right, when sum < 0, reset sum to 0.
*/

#include <stdio.h>

int array[] = {1, -2, 3, 10, -4, 7, 2, -5};

int maxSubArray(int *a, int size, int *start, int *end)
{
    int sum = 0;
    int cur = 0;
    int max = -(1<<31);

    if (size <= 0) {
        return -1;
    }

    while(cur < size) {
        sum += a[cur];
        if(sum > max) {
            max = sum;
            *end = cur;
            cur++;
        } else if(sum <=0){
            sum = 0;
            cur++;
            // if (cur < size) {
                *start = cur;
                *end = cur;
            // } else {

            // }

        }  else {
            cur++;
        }
    }

    return max;
}

int main(int argc, char *args[])
{
    int max;
    int start, end;

    max = maxSubArray(array, 8, &start, &end);
    printf("max is %d, from %d to %d\n", max, start, end);
    while(start <= end) {
        printf("%d\t", array[start++]);
    }

    return 0;
}