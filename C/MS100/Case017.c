/*
第17 题：
题目：在一个字符串中找到第一个只出现一次的字符。如输入abaccdeff，则输出b。
分析：这道题是2006 年google 的一道笔试题。
*/
#include <stdio.h>
#include <stdlib.h>

char s[] = "abaccdeff";

char first_single(char *string)
{
    int array[255] = {0};
    char *cur = string;

    while(*cur != '\0') {
        array[*cur]++;
        cur++; 
    }

    cur = string;
    while(*cur != '\0') {
        if (array[*cur] == 1) {
            return *cur;
        }
        cur++;
    }

    return 0;
}

int main(void)
{
    char first;
    char string[100];

    printf("Please input string:");
    scanf("%s", string);
    printf("get the string:%s\n", string);
    first = first_single(string);
    if (first)
        printf("The first single char is %c\n", first);
    else
        printf("no result!");

    return 0;
}
