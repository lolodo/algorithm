/*
5.查找最小的k 个元素
题目：输入n 个整数，输出其中最小的k 个。
例如输入1，2，3，4，5，6，7 和8 这8 个数字，则最小的4 个数字为1，2，3 和4。
ANSWER:
This is a very traditional question...
O(nlogn): cat I_FILE | sort -n | head -n K
O(kn): do insertion sort until k elements are retrieved.
O(n+klogn): Take O(n) time to bottom-up build a min-heap. Then sift-down k-1 times.
So traditional that I don’t want to write the codes...
Only gives the siftup and siftdown function.
*/

#include <stdio.h>

#define MIN_NUM 4
int array[] = {1,6,23,456,43,12,87,2323, 636};

int max[MIN_NUM] = {0};

void find_least_num(int *num, int len)
{
    // It's so easy, so skip it!
}

int main(int argc, char *args[])
{

}