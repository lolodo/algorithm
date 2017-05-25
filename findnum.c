// 题目1：编程题，给出o(n平方)的算法，同时把编程代码完整写出来。不能使用Hash算法。
// 给定N个链表，每个链表的值单调递增，而且都是正数。求在所有链表中都存在的一个数。没有这样的数，返回-1

// 函数签名： int findCommon(int** list_nod, int length);

// 例：
// 1 2 3 4 5 7
// 2 3 5 6 7
// 1 4 5 7 9

// 所有链表都存在的数为 5 和 7， 找出其中一个即可

#include "list.h"

struct list_node
{
    int val;
    struct list_node *next;
};

int list1[] = {1, 2, 3, 4, 5, 7};
// int list1[] = {};
int list2[] = {2, 3, 5, 6, 7};
int list3[] = {1, 4, 5, 7, 9};

struct list_node *init_list(int *list, int size)
{
    struct list_node *head, *tmp, *last;
    int i, n;

    n = size / 4;
    head = malloc(n * sizeof(struct list_node));
    tmp = head;

    for (i = 0; i < n; i++)
    {
        tmp->val = *(list + i);
        tmp->next = tmp + 1;
        last = tmp;
        tmp++;
    }
    last->next = NULL;

    return head;
}

void free_list(struct list_node *node)
{
    free((void *)node);
}

void print_list(struct list_node *node)
{
    while (node)
    {
        printf("%d\t", node->val);
        node = node->next;
    }
    printf("\n");
}

int compare_list(struct list_node **list1, struct list_node **list2)
{
    int i, j;
    int equal = -1;
    struct list_node *head, *next;

    if (!list1)
        return -1;

    if (!list2)
        return -1;

    head = *list1;
    next = *list2;

    if (!head)
        return -1;

    if (!next)
        return -1;

    while (next && head) {
        // printf("head:%d, next:%d\n", head->val, next->val);
        if (head->val > next->val) {
            // printf("bigger!\n");
            next = next->next;
            *list2 = next;
        } else if (head->val < next->val) {
            head = head->next;
            // printf("smaller!\n");
            *list1 = head;
        } else {
            // printf("equal!\n");
            equal = head->val;
            break;
        }
    }

    return equal;
}

int find_common(struct list_node **list_node, int length)
{
    int common_cur = -1;
    int common_last = -1;
    int length_tmp = 0;
    int common = -1;
    struct list_node *head, *next, **tmp, **judge;

    if (!list_node) {
        printf("list node is null!\n");
        return -1;
    }
    
    if (length < 2) {
        printf("length is less than 2!\n");
        return -1;
    }
    
    head = *list_node;
    if (!head) {
        printf("head is null!\n");
        return -1;
    }

    judge = list_node;
    while (*(judge++)) {
        length_tmp++;
    }

    if (length != length_tmp) {
        printf("length is error, the max length is %d, it will be changed to 3!\n", length_tmp);
        length = length_tmp;
    }

    list_node++;
    while (head && (common < 0)) {
        next = *list_node;
        tmp = list_node;
        length_tmp = length;
        common_last = compare_list(&head, &next);
        if (common_last > 0) {
            tmp++;
            next = *tmp;
        } else {
            break;
        }

        if (length_tmp == 2) {
            common = common_last;
            // break;
        }

        if (!tmp) {
            printf("node is null!\n");
            return -1;
        }
        
        // printf("common_last:%d\n", common_last);
        while (next && (--length_tmp >= 2) && tmp){
            common_cur = compare_list(&head, &next);
            if (common_cur == common_last) {
                if (length_tmp == 2) {
                    common = common_cur;
                }
                // printf("common_cur:%d\n", common_cur);
                // printf("len:0x%x\n", length_tmp);
                length_tmp--;
                tmp++;
                next = *tmp;
            } else {
                break;
            }
        }
    }

    return common;
}

int main(void)
{
    struct list_node **tmp;
    int i, common = -1;
    struct list_node *n_list[4];
    
    if (sizeof(list1)) {
        n_list[0] = init_list(list1, sizeof(list1));
        // n_list[0] = NULL; //just for test
    } else {
        return -1;
    }

    if (sizeof(list2)) {
        n_list[1] = init_list(list2, sizeof(list2));  
    } else {
        return -1;
    }

    if (sizeof(list3)) {
        n_list[2] = init_list(list3, sizeof(list3));
    } else {
        return -1;
    }

    n_list[3] = NULL;

    tmp = n_list;
    while (*tmp)
    {
        print_list(*tmp);
        tmp++;
    }

    common = find_common(n_list, 3);
    if (common > 0) {
        printf("found====>>>>%d\n", common);
    } else {
        printf("Not found!\n");
    }

    tmp = n_list;
    while (*tmp)
    {
        free_list(*tmp);
        tmp++;
    }
    
    return 0;
}