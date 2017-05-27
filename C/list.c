#include "list.h"

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

int main(void)
{
    return 0;
}