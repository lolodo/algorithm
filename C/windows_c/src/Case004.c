/*
4.在二元树中找出和为某一值的所有路径
题目：输入一个整数和一棵二元树。
从树的根结点开始往下访问一直到叶结点所经过的所有结点形成一条路径。
打印出和与输入整数相等的所有路径。
例如输入整数22 和如下二元树
10
/ \
5 12
/ \
4 7
则打印出两条路径：10, 12 和10, 5, 7。
二元树节点的数据结构定义为：
struct BinaryTreeNode // a node in the binary tree
{
int m_nValue; // value of node
BinaryTreeNode *m_pLeft; // left child of node
BinaryTreeNode *m_pRight; // right child of node
};
*/

#include <stdio.h>

struct BSTreeNode {
    int val;
    struct BSTreeNode *left;
    struct BSTreeNode *right; 
};

struct BSTreeNode node0, node1, node2, node3, node4;
int search_val = 22;

void BSTreeNodeInit(void) {
    node0.val = 10;
    node1.val = 5;
    node2.val = 12;
    node3.val = 4;
    node4.val = 7;

    node0.left = &node1;
    node0.right = &node2;
    node1.left = &node3;
    node1.right = &node4;
    node2.left = NULL;
    node2.right = NULL;
    node3.left = NULL;
    node3.right = NULL;
    node4.left = NULL;
    node4.right = NULL;
}
int printPaths(struct BSTreeNode *root, int sum) 
{
    int ret;
    int flag = 0;

    if(!root) {
        return -1 ;
    }

    sum += root->val;
    if(sum > search_val) {
        return -1;
    }
    
    if(sum == search_val) {
        printf("%d ", root->val);
        return 1;
    }

    // printf("%d ", root->val);
    ret = printPaths(root->left, sum);
    if(ret == 1) {
        printf("%d ", root->val);
        // return 1;
        flag = 1;
    }

    ret = printPaths(root->right, sum);
    if(ret == 1) {
        printf("%d ", root->val);
        // return 1;
        flag = 1;
    }

    return flag;
}
int main(int argc, char *args[])
{
    BSTreeNodeInit();
    search_val = 22;
    printPaths(&node0, 0);
    return 0;
}