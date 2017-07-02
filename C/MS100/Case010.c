/*
翻转句子中单词的顺序。
题目：输入一个英文句子，翻转句子中单词的顺序，但单词内字符的顺序不变。
句子中单词以空格符隔开。为简单起见，标点符号和普通字母一样处理。
例如输入“I am a student.”，则输出“student. a am I”。
Answer:
Already done this. Skipped.
*/

#include <stdio.h>
#include <string.h>

void reverse(char *start, char *end) {
    if(start == NULL || end == NULL) {
        return;
    }

    while(start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++, end--;
    }
}

void reverse_sentence(char *string)
{
    int len;
    int i, j;
    int start, end;
    char *start_c, *end_c;
    char c;

    if (string == NULL) {
        printf("string is null!\n");
        return;
    }

    len = strlen(string);
    if (len <= 1) {
        printf("this string only has one character!\n");
        return;
    }

    start_c = string;
    end_c = string + len - 1;
    reverse(start_c, end_c);

    i = 0;
    while(1) {
        while(string[i] == ' ') {
            i++;
        }

        start = i;
        
        while(string[i] != ' ' && string[i] != '\0') {
            i++;
        }

        end = i;

        start_c = string + start;
        end_c = string + end - 1;
        reverse(start_c, end_c);

        if (string[i] == '\0'){
            break;
        }
    }
}

int main(int argc, char *args[])
{
    // char s1[] = "I am a studdent.";
    // char s1[]= "   ";
    
    reverse_sentence(args[1]);
    printf("1:%s\n", args[1]);

    return 0;
}