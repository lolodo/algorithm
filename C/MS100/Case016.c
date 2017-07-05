/*
题目（微软）：
输入一颗二元树，从上往下按层打印树的每个结点，同一层中按照从左往右的顺序打印。
例如输入
8
/ \
6 10
/ \ / \
5 7 9 11
输出8 6 10 5 7 9 11。
*/

#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

struct BSTreeNode *a[100] = {NULL};

void print_by_level(struct BSTreeNode *root)
{
    int i = 0;
    int j = 0;
    // struct BSTreeNode *cur;

    if (root == NULL) {
        return;
    }

    a[i] = root;
    j = i + 1;
    while((i < j) && (j < 100)) {
        // cur = a[i];
        printf("%d ", a[i]->val);
        // i++;

        if(a[i]->left) {
            a[j++] = a[i]->left;
        }

        if (a[i]->right) {
            a[j++] = a[i]->right;
        }

        i++;
    }
}

int main(void)
{
    int number;
    struct BSTreeNode *node;

    printf("Please input number:");
    scanf("%d", &number);
    node = BSTreeNodeInit(number);
    PrintTree(node);
    printf("\n");

    print_by_level(node);
    printf("\n");
}

