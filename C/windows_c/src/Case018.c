/*
题目：n 个数字（0,1,…,n-1）形成一个圆圈，从数字0 开始，
每次从这个圆圈中删除第m 个数字（第一个为当前数字本身，第二个为当前数字的下一个数
字）。
当一个数字删除后，从被删除数字的下一个继续删除第m 个数字。
求出在这个圆圈中剩下的最后一个数字。
July：我想，这个题目，不少人已经见识过了。
ANSWER:
Actually, although this is a so traditional problem, I was always to lazy to think about this or even to search for the answer.(What a shame...). Finally, by google I found the elegant solution for it.
The keys are:
1) if we shift the ids by k, namely, start from k instead of 0, we should add the result by k%n
2) after the first round, we start from k+1 ( possibly % n) with n-1 elements, that is equal to an (n-1) problem while start from (k+1)th element instead of 0, so the answer is (f(n-1, m)+k+1)%n
3) k = m-1, so f(n,m)=(f(n-1,m)+m)%n. 
finally, f(1, m) = 0;
Now this is a O(n) solution.
*/

#include <stdio.h>
#include <stdlib.h>

int joseph(int n, int m)
{
    int fn = 0;

    for (int i = 2; i <= n; i++)
    {
        fn = (fn + m) % i;
    }
    
    return fn;
}

int main(void)
{
    int n, m;
    int ret;

    printf("Please input n:");
    scanf("%d", &n);

    printf("Please input m:");
    scanf("%d", &m);

    ret = joseph(n, m);
    printf("ret is %d!\n", ret);
    
    return 0;
}