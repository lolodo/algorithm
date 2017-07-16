/*
 * 输入一个整数n，求从1 到n 这n 个整数的十进制表示中1出现的次数。
 * 例如输入12，从1 到12 这些整数中包含1 的数字有1，10，11 和12
 ⼀共 出现了了5 次。
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
    int count = 0;
    int num = number;
    int remain;
    int array[100] = {0};
    int power_num;

    while (num){
        num /= 10;
        count = power(10, dec) + count * 8;
        sum += count;
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
/*
Suppose we have N=ABCDEFG. 
if G<1, # of 1’s in the units digits is ABCDEF, 
else ABCDEF+1 
if F<1, # of 1’s in the digit of tens is (ABCDE)*10, 
else if F==1: (ABCDE)*10+G+1, 
else (ABCDE+1)*10 
if E<1, # of 1’s in 3rd digit is (ABCD)*100, 
else if E==1: (ABCD)*100+FG+1, 
else (ABCD+1)*100 … so on. 
if A=1, # of 1 in this digit is BCDEFG+1, 
else it’s 1*1000000; 
so to fast access the digits and helper numbers, 
we need to build the fast access table of preﬁxes and suﬃxes.
*/

int count_of_1s(int n)
{
    int prefix[10], suffix[10] = {0};
    int digits[10], counts[10];
    int i = 0;
    int div;
    int remain = 0;
    int count = 0;
    int j = 0;
    int sum = 0;

    (n > 0)? n: n*(-1);
    
    if (!n) {
        return 0;
    }

    div = n;
    while (1) {
        digits[i] = div % 10;
        div /= 10;
        prefix[i] = div;

        if (!count) {
            count = 1;
        } else {
            count *= 10;
        }

        counts[i] = count;
        remain += count * digits[i];
        suffix[i + 1] = remain; 

        if (div) {
            i++;
        } else {
            break;
        }
    }

    while (j <= i) {
        printf("%d:suffix:%d, prefix:%d\n", j, suffix[j], prefix[j]);
        j++;
    }

    j = 0;
    while (j < i) {
        if (digits[j] == 0) {
            sum += prefix[j] * counts[j];
        } else if (digits[j] == 1) {
            sum += prefix[j] * counts[j] + suffix[j] + 1;
        } else {
            sum += (prefix[j] + 1) * counts[j];
        }
        j++;
    }

    if (digits[i] == 1) {
        sum += prefix[i] * counts[i] + suffix[i] + 1;
    } else {
        sum += counts[i];
    }

    return sum;
}

int get_ones_count(int n)
{
    int prefix[10], suffix[10], digit[10];
    int i = 0;
    int base = 1;

    while (base < n) {
        suffix[i] = n % base;
        digit[i] = (n % (base * 10)) - suffix[i];
        prefix[i] = n - suffix[i] - digit[i];
        i++, base *= 10;
    }

    int count = 0;
    int pre;
    base = 1;
    for (int j = 0; j < i; j++){
        pre = prefix[j] /  10;
        if (digit[j] < base) {
            count += pre;
        } else if (digit[j] == base) {
            count += pre + suffix[j] + 1;
        } else {
            count += pre + base;
        }

        base *= 10;
    }

    return count;
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
    
    // ret = find_one(number);
    // ret = count_of_1s(number);
    // printf("%d has %d ones!\n", number, ret);

    ret = get_ones_count(number);
    printf("get one:%d has %d ones!\n", number, ret);

    return ret; 
}
