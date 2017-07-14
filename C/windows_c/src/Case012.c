/*
题目：求1+2+…+n，
要求不能使用乘除法、for、while、if、else、switch、case 等关键字以及条件判断语句
（A?B:C）。
*/

#include <stdio.h>
#include <stdlib.h>

int sum_all(int number)
{    
    int ret = 0;

    (number <= 0) || (ret = sum_all(number - 1));

    return number + ret;
}

int main(int argc, char *args[]) 
{
    int number;
    int sum;

    printf("please input number:");
    scanf("%d", &number);

    sum = sum_all(number);
    printf("The sum is %d\n", sum);

    return sum;
}
