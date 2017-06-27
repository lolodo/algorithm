/*
56.最长公共子序列。
题目：如果字符串一的所有字符按其在字符串中的顺序出现在另外一个字符串二中，
则字符串一称之为字符串二的子串。

注意，并不要求子串（字符串一）的字符必须连续出现在字符串二中。
请编写一个函数，输入两个字符串，求它们的最长公共子串，并打印出最长公共子串。
例如：输入两个字符串BDCABA和ABCBDAB，字符串BCBA和BDAB都是是它们的最长公共子序列，则输出它们的长度4，并打印任意一个子序列。
*/
#include <stdio.h>
#include <string.h>

char *s1 = "BDCABA";
char *s2 = "ABCBDAB";
int slen, flen;
int c[6][7] = {0};

int max(int a, int b) {
    return (a > b)? a:b;
}

int lcs(char *first, int f_len, char  *second, int s_len)
{
    int ret;

    if (!first || !second) {
        return 0;
    }

    if (f_len < 0 || s_len < 0) {
        return 0;
    }

    if (f_len == 0 && s_len == 0) {
        if (first[0] == second[0]) {
            printf("%c ", first[0]);
            return 1;
        }

        // printf("The longest length:%d\n", sum);
        return 0;
    }

    if (first[f_len] == second[s_len]) {
        ret = lcs(first, f_len - 1, second, s_len - 1) + 1;
        // if ((slen >= s_len) && (flen >= f_len)) {
        if(!c[f_len][s_len]) {
            printf("%c f:%d, s:%d\n", first[f_len], f_len, s_len);
            c[f_len][s_len] = 1;
        }
            // slen = s_len;
            // flen = f_len;
        // }
        return ret;
    } else {
        int ret0, ret1;
        ret0 = lcs(first, f_len - 1, second, s_len);
        ret1 = lcs(first, f_len, second, s_len - 1);
        ret = max(ret0, ret1);
        return ret;
    }

    return 0;
}

int main(int argc, char *args[])
{
    int len;

    slen = strlen(s2) - 1;
    flen = strlen(s1) - 1;

    // len = strlen(s1);
    len = lcs(s1, strlen(s1) - 1, s2, strlen(s2) - 1);
    printf("\nlen is %d\n", len);

    return 0;
}