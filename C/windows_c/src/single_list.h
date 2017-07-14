#ifndef __SINGLE_LIST_H__
#define __SINGLE_LIST_H__

struct ListNode
{
    int m_nKey;
    struct ListNode *m_pNext;
};

struct ListNode *get_list(int number);
void print_list(struct ListNode *head);
struct ListNode *case013(int number);

#endif //__SINGLE_LIST_H__
