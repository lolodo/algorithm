/*
题目：输入一个单向链表，输出该链表中倒数第k 个结点。链表的倒数第0 个结点为链表的尾指针。
链表结点定义如下：
struct ListNode
{
    int m_nKey;
    ListNode* m_pNext;
};
*/

#include <stdio.h>
#include <stdlib.h>
#include "single_list.h"

struct ListNode *case013(int number)
{
    int list_number;
    struct ListNode *head;
    struct ListNode *first, *second;
    int counter = number;
    
    if (number < 0) {
        return NULL;
    }

    printf("please input list number:");
    scanf("%d", &list_number);
    head = get_list(list_number);

    if (head == NULL) {
        return NULL;
    }

    print_list(head);
    first = head;
    second = head;
    while (counter--) {
        first = first->m_pNext;
        if (first == NULL) {
            return NULL;
        }
    }

    while(first) {
        first = first->m_pNext;
        second = second->m_pNext;
    }

    return second;
}
