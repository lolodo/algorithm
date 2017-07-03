/* 
the first tree:
     10
   /   \
  6     14
 / \   /  \
4   8 12  16
   /   \
  23   34

the second tree:
            10
           /  \
          6    14
         / \  
        4   8 
       /     \  
      34     23
     /         \
    67         54

the third tree:
        21
       /  \
      16   1
     /  
    48  
   /   
  3   
 /     
6    

      
*/
#include <stdio.h>
#include "binary_tree.h"

/* The first tree */
static struct BSTreeNode node0, node1, node2, node3, node4, node5, node6, node7, node8;

/* The second tree */
static struct BSTreeNode n0, n1, n2, n3, n4, n5, n6, n7, n8;

static int flag0 = 0;
static int flag1 = 0;
static int flag2 = 0;

struct BSTreeNode *first_tree(void)
{
    node0.val = 10;
    node1.val = 6;
    node2.val = 14;
    node3.val = 4;
    node4.val = 8;
    node5.val = 12;
    node6.val = 16;
    node7.val = 23;
    node8.val = 34;

    node0.left = &node1;
    node0.right = &node2;
    node1.left = &node3;
    node1.right = &node4;
    node2.left = &node5;
    node2.right = &node6;
    node3.left = NULL;
    node3.right = NULL;
    node4.left = &node7;
    node4.right = NULL;
    node5.left = NULL;
    node5.right = &node8;
    node6.left = NULL;
    node6.right = NULL;
    node7.left = NULL;
    node7.right = NULL;
    node8.left = NULL;
    node8.right = NULL;

    return &node0;
}


static struct BSTreeNode n30, n31, n32, n33, n34, n35;
struct BSTreeNode *second_tree(void)
{
    n30.val = 10;
    n31.val = 6;
    n32.val = 14;
    n33.val = 4;
    n4.val = 8;
    n5.val = 34;
    n6.val = 23;
    n7.val = 67;
    n8.val = 54;
    
    n0.left = &n1;
    n0.right = &n2;
    n1.left = &n3;
    n1.right = &n4;
    n2.left = NULL;
    n2.right = NULL;
    n3.left = &n5;
    n3.right = NULL;
    n4.left = NULL;
    n4.right = &n6;
    n5.left = &n7;
    n5.right = NULL;
    n6.left = NULL;
    n6.right = &n8;
    n7.left = NULL;
    n7.right = NULL;
    n8.left = NULL;
    n8.right = NULL;
    
    return &n0;

}

struct BSTreeNode *tree3(void)
{
    n30.val = 21;
    n31.val = 16;
    n32.val = 1;
    n33.val = 48;
    n34.val = 3;
    n35.val = 6;
    
    n30.left = &n31;
    n30.right = &n32;
    n31.left = &n33;
    n31.right = NULL;
    n32.left = NULL;
    n32.right = NULL;
    n33.left = &n34;
    n33.right = NULL;
    n34.left = &n35;;
    n34.right = NULL;
    n35.left = NULL;
    n35.right = NULL;
    
    return &n30;

}


struct BSTreeNode *BSTreeNodeInit(int num) 
{
    if (num == 0 && flag0 == 0) {
        return first_tree();
    } else if(num == 1 && flag1 == 0) {
        return second_tree();
    } else if(num == 2 && flag2 == 0) {
        return tree3();
    } else {
        if (num > 2) {
            printf("number is bigger than 1!===>> %d\n", num);
        } else {
            printf("It's already initialized!\n");
        }

        return (struct BSTreeNode *)NULL;
    }
}

void PrintTree(struct BSTreeNode *head)
{
    if (head) {
        printf("%d ", head->val);
        PrintTree(head->left);
        PrintTree(head->right);
    }
}
