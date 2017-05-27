#ifndef __LIST_H
#define __LIST_H

#include <stdio.h>
#include <stdlib.h>

struct list_node
{
    int val;
    struct list_node *next;
};

struct list_node *init_list(int *list, int size);
void free_list(struct list_node *node);
void print_list(struct list_node *node);
#endif