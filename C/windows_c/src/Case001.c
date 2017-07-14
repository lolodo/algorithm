/* 
1.把二元查找树转变成排序的双向链表（树）
 题目：
输入一棵二元查找树，将该二元查找树转换成一个排序的双向链表。
要求不能创建任何新的结点，只调整指针的指向。
    10
   /  \
  6    14
 / \   /  \
4   8  12 16
 转换成双向链表
4=6=8=10=12=14=16
*/
#include <stdio.h>

struct BSTreeNode {
    int val;
    struct BSTreeNode *left;
    struct BSTreeNode *right; 
};

enum  NodePos {
    BST_LEFT,
    BST_RIGHT
};

struct BSTreeNode node0, node1, node2, node3, node4, node5, node6;

struct BSTreeNode *list = NULL;

void BSTreeNodeInit(void) {
    node0.val = 10;
    node1.val = 6;
    node2.val = 14;
    node3.val = 4;
    node4.val = 8;
    node5.val = 12;
    node6.val = 16;

    node0.left = &node1;
    node0.right = &node2;
    node1.left = &node3;
    node1.right = &node4;
    node2.left = &node5;
    node2.right = &node6;
    node3.left = NULL;
    node3.right = NULL;
    node4.left = NULL;
    node4.right = NULL;
    node5.left = NULL;
    node5.right = NULL;
    node6.left = NULL;
    node6.right = NULL;
}

void PrintTree(struct BSTreeNode *head)
{
    if (head) {
        printf("%d ", head->val);
        PrintTree(head->left);
        // printf("%d ", head->val);
        PrintTree(head->right);
    }
}


void PrintList(struct BSTreeNode *head)
{
    while(head) {
        printf("%d ", head->val);
        head = head->right;
    }
}

void convert_node(struct BSTreeNode *root)
{
    struct BSTreeNode *cur;

    if(!root) {
        return;
    }

    // cur = root;
    // if(cur->left) {
    convert_node(root->left);
    // }

    root->left = list;
    if(list) {
        list->right = root;
    }
    list = root;

    // if (cur->right) {
    convert_node(root->right);
    // }

}

struct BSTreeNode * convert_solution(struct BSTreeNode * root)
{
    // struct BSTreeNode *head = NULL;

    convert_node(root);

    // head = list;
    while(list && list->left) {
        list = list->left;
    }

    return list;
}

int main(int argc, char *args[])
{
    struct BSTreeNode *list = NULL;
    BSTreeNodeInit();

    PrintTree(&node0);
    printf("\n");

    list = convert_solution(&node0);
    PrintList(list);
    printf("\n");

    return 0;
}