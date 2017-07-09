/*
题目：输入一个表示整数的字符串，把该字符串转换成整数并输出。
例如输入字符串"345"，则输出整数345。
ANSWER:
This question checks how the interviewee is familiar with C/C++? I’m so bad at C/C++...

int atoi(char * str) {
  int neg = 0;
  char * p = str;
  if (*p == ‘-’) {
    p++; neg = 1;
  } else if (*p == ‘+’) {
    p++;
  }
  int num = 0;
  while (*p != ‘\0’) {
    if (*p>='0' && *p <= '9') {
      num = num * 10 + (*p-’0’);
    } else {
      error(“illegal number”);
    }
    p++;
  }
  return num;
}
PS: I didn’t figure out how to tell a overflow problem easily.
*/

#include <stdio.h>
#include <stdlib.h>

int new_atoi(char *str, int *out)
{
    int neg = 0;
    char *p;
    int num = 0;

    p = str;

    if ( *p == '-') {
        neg = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    while (*p != '\0') {
        if (*p >= '0' && *p <= '9') {
            num = num * 10 + *p - '0';
            p++;
        } else {
            return -1;
        }
    }
    
    *out = neg? (-1) * num: num;

    return 0;
}
int main(void)
{
    char string[100];
    int ret, num;

    printf("please input legal number:");
    scanf("%s", string);

    printf("the string:%s\n", string);
    ret = new_atoi(string, &num);
    if (ret) {
        return -1;
    }

    printf("The result is %d\n", num);
    return 0;
}