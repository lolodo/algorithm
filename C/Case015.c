/*
题目：输入一颗二元查找树，将该树转换为它的镜像，
即在转换后的二元查找树中，左子树的结点都大于右子树的结点。
用递归和循环两种方法完成树的镜像转换。
例如输入：

    8
  /   \
 6    10
/ \   / \
5 7  9  11
输出：
    8
  /   \
 10    6
/ \    /\
11 9  7  5
定义二元查找树的结点为：
*/

#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

void swap_bstree(struct BSTreeNode *root)
{
    struct BSTreeNode *cur = NULL;

    cur = root->left;
    root->left = root->right;
    root->right = cur;
}



void reverse_bstree(struct BSTreeNode *root)
{
    if (!root) {
        return ;
    }

    swap_bstree(root);
    reverse_bstree(root->left);
    reverse_bstree(root->right);
}

int main(void)
{
    int tree;
    struct BSTreeNode *node;

    printf("Please input number:");
    scanf("%d", &tree);

    node = BSTreeNodeInit(tree);
    PrintTree(node);
    printf("\n");

    reverse_bstree(node);
    PrintTree(node);
    printf("\n");

    return 0;
}
