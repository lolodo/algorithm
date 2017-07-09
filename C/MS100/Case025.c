/*
写一个函数,它的原形是int continumax(char *outputstr,char *intputstr)
功能：
在字符串中找出连续最长的数字串，并把这个串的长度返回，
并把这个最长数字串付给其中一个函数参数outputstr 所指内存。
例如："abcd12345ed125ss123456789"的首地址传给intputstr 后，函数将返回9，
outputstr 所指的值为123456789
ANSWER:

int continumax(char *outputstr, char *inputstr) {
  int len = 0;
  char * pstart = NULL;
  int max = 0;
  while (1) {
    if (*inputstr >= ‘0’ && *inputstr <=’9’) {
      len ++;
    } else {
      if (len > max) pstart = inputstr-len;
      len = 0;
    }
    if (*inputstr++==’\0’) break;
  }
  for (int i=0; i<len; i++)
    *outputstr++ = pstart++;
  *outputstr = ‘\0’;
  return max;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int continumax(char *outputstr, char *inputstr)
{
    char *start;
    int max = 0;
    int max_cur = 0;
    char *cur = inputstr;
    int i;

    while (1)
    {
        if (*cur >= '0' && *cur <= '9') {
            max_cur++;
        } else {
            if (max_cur > max) {
                start = cur - max_cur;
                max = max_cur;
            }
            max_cur = 0;            
        }

        if (*cur == '\0') {
            break;
        }
        cur++;
    }

    for(i = 0; i < max; i++) {
        *outputstr++ = *start++;
    }

    *outputstr = '\0';

    return max;
}

int main(void)
{
    int len;
    char input_str[100];
    char output_str[100];

    printf("Please input string:");
    scanf("%s", input_str);

    len = continumax(output_str, input_str);
    if (len) {
        printf("The length:%d, string:%s\n", len, output_str);
    }

    return 0;
}