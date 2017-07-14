#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

struct BSTreeNode {
    int val;
    struct BSTreeNode *left;
    struct BSTreeNode *right; 
};

struct BSTreeNode *BSTreeNodeInit(int num);
void PrintTree(struct BSTreeNode *head);
int get_height(struct BSTreeNode *root);
void case011(struct BSTreeNode *root);
#endif //__BINARY_TREE_H__
