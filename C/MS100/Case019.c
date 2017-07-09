/*
题目：定义Fibonacci 数列如下：
/ 0 n=0
f(n)= 1 n=1
\ f(n-1)+f(n-2) n=2
输入n，用最快的方法求该数列的第n 项。
分析：在很多C 语言教科书中讲到递归函数的时候，都会用Fibonacci 作为例子。
因此很多程序员对这道题的递归解法非常熟悉，但....呵呵，你知道的。。
ANSWER:
This is the traditional problem of application of mathematics...
let A=
{1 1}
{1 0}
f(n) = A^(n-1)[0,0]
this gives a O(log n) solution.
int f(int n) {
  int A[4] = {1,1,1,0};
  int result[4];
  power(A, n, result);
  return result[0];
}

void multiply(int[] A, int[] B, int _r) {
  _r[0] = A[0]*B[0] + A[1]*B[2];
  _r[1] = A[0]*B[1] + A[1]*B[3];
  _r[2] = A[2]*B[0] + A[3]*B[2];
  _r[3] = A[2]*B[1] + A[3]*B[3];
}

void power(int[] A, int n, int _r) {
  if (n==1) { memcpy(A, _r, 4*sizeof(int)); return; }
  int tmp[4];
  power(A, n>>1, _r);
  multiply(_r, _r, tmp);
  if (n & 1 == 1) {
    multiply(tmp, A, _r);
  } else {
    memcpy(_r, tmp, 4*sizeof(int));
  }
}
*/
#include <stdio.h>
#include <stdlib.h>

int fibonacci(int n)
{

    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        int fn[3] = {0};

        fn[0] = 0;
        fn[1] = 1;

        while (n >= 2) {
            fn[2] = fn[0] + fn[1];
            fn[0] = fn[1];
            fn[1] = fn[2];
            n--;
        }

        return fn[2];
    }
}
/*
------               ----
f[n+1]  = [ 1, 1] ^n f[1]
f[n]      [ 1, 0]    f[0]
------               ----
*/

unsigned int fibonacci2(unsigned int n)
{
    if (n == 0) {
        return 0;
    } else if(n == 1) {
        return 1;
    } else {
        int a[4] = {1, 1, 1, 0};
        int ret[4] = {1, 1, 1, 0};
        
        while (n-- > 1) {
            ret[0] = ret[0] * a[0] + ret[1] * a[2];
            ret[1] = ret[0] * a[1] + ret[1] * a[3];
            ret[2] = ret[2] * a[0] + ret[3] * a[2];
            ret[3] = ret[2] * a[1] + ret[3] * a[3];
        }

        return ret[2];
    }

}

int main(void)
{
    int n;
    int ret;

    printf("Please input number:");
    scanf("%d", &n);

    ret = fibonacci(n);
    printf("1.The result is %d\n", ret);
    
    ret = fibonacci2(n);
    printf("2.The result is %d\n", ret);

    return 0;
}