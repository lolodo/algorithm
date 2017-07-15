/*
 * 输入一个整数n，求从1 到n 这n 个整数的十进制表示中1出现的次数。
 * 例如输入12，从1 到12 这些整数中包含1 的数字有1，10，11 和12，1 ⼀共 出现了了5 次。
 * */

#include <stdio.h>
#include <stdlib.h>

int power(int n, int idx) {
    int ret = 1;

    if (idx == 0) {
        return 1;
    }

    while(idx--) {
        ret *= n;     
    }

    return ret;
}

int find_one(int number)
{
    //10:1~9之间有1个1 f(0) = 1
    //10~99之间有10~19（10个）， 20~99（8 * (f(10))） f(1) = 10 ^1 + 8*f(0)
    //100~999之间有100 ~199 （100个）, 200 ~ 999 (8 * (f(100))) f(2) = 10 ^ 2 + 8 * f(1) 
    //f(n) = 10 ^ n + 8 * f(n - 1)
    //sum = f(0) + f(1) + f(2) + f(3) + f(4) + f(5) .... f(n - 1) + f(n)
    // 232 = 200 + 30 + 2
    // 2 > 1 f(0) = 1 30=> 0~9 10 ~19 20 ~30
    //200 = f(2) + 10 ^ 2
    int sum = 0;
    int dec = 0;
    int i;
    int counter = 0;
    int num = number;
    int remain;
    int array[100] = {0};
    int power_num;

    while (num){
        num /= 10;
        counter = power(10, dec) + counter * 8;
        sum += counter;
        array[dec + 1] = sum;
        dec++;
    }

    sum = 0;
    remain = number;
    while (dec--) {
        int flag;
        if (dec <= 0) {
            sum += 1;
            return sum;
        }

        sum += array[dec];
        power_num = power(10, dec);
        remain -= 2 * power_num;
        if (remain < 0) {
            remain += power_num;
            sum += remain + 1;
            return sum;
        }

        sum += power(10, dec) + (remain / power_num) * array[dec];
        remain %= power_num;
        // dec--;
    }

    return sum;
}

int main()
{
    int number;
    int ret;

    printf("Please input number:");
    scanf("%d", &number);
    
    if (number < 1) { 
        return 0;
    }
    
    ret = find_one(number);
    printf("%d has %d ones!\n", number, ret);

    return ret; 
}
