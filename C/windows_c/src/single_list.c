/*
 20->4->78->19->34->3->9->71->45->34
*/

#include <stdio.h>
#include "single_list.h"

static struct ListNode l0, l1, l2, l3, l4, l5, l6, l7, l8, l9;
struct ListNode *list0(void)
{
    l0.m_nKey = 20;
    l1.m_nKey = 4;
    l2.m_nKey = 78;
    l3.m_nKey = 19;
    l4.m_nKey = 34;
    l5.m_nKey = 3;
    l6.m_nKey = 9;
    l7.m_nKey = 71;
    l8.m_nKey = 45;
    l9.m_nKey = 34;

    l0.m_pNext = &l1;
    l1.m_pNext = &l2;
    l2.m_pNext = &l3;
    l3.m_pNext = &l4;
    l4.m_pNext = &l5;
    l5.m_pNext = &l6;
    l6.m_pNext = &l7;
    l7.m_pNext = &l8;
    l8.m_pNext = &l9;
    l9.m_pNext = NULL;

    return &l0;
}

struct ListNode *get_list(int number)
{
    if (number == 0) {
        return list0();
    } else {
        return NULL;
    }

}

void print_list(struct ListNode *head)
{
    while (head) {
        printf("%d ", head->m_nKey);
        head = head->m_pNext;
    }
    printf("\n");
}
