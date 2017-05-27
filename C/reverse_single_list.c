#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

#define NUMBER 4

int value[NUMBER] = {1, 2 , 3, 4};

struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode *cur, *next;
    
    if (!head || !(head->next)) {
        return head;    
    }
    
    cur = head->next;
    head->next = (struct ListNode *)0;
    
    while(cur) {
        next = cur->next;
        cur->next = head;
        head = cur;
        cur = next;
    }
    
    return head;
}

// int main(int argc, char **argv[])
int main(void)
{
    struct ListNode *head, *tmp, *last;
    int i;

    head = malloc((NUMBER + 1) * sizeof(struct ListNode));
    tmp = head;

    for (i = 0; i < NUMBER; i++, tmp++)
    {
        tmp->val = value[i];
        // printf("%d\n", tmp->val);
        tmp->next = (tmp + 1);
        last = tmp;
    }
    last->next = NULL;

    printf("before===>>>\n");
    tmp = head;
    while(tmp) {
        printf("%d\t", tmp->val);
        tmp = tmp->next;
    }
    printf("\n");

    tmp = reverseList(head);
    printf("after===>>>\n");
    while(tmp) {
        printf("%d\t", tmp->val);
        tmp = tmp->next;
    }
    printf("\n");

    return 0;
}