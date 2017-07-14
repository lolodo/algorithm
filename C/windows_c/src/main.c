#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"
#include "single_list.h"

int main(int argc, char *args[]) 
{
    int i, height;
    struct BSTreeNode *node;
    int case_num;

    printf("chose cases:");
    scanf("%d", &case_num);
    if (case_num < 0) {
        printf("must bigger than 0!\n");
        return -1;
    }

    if (case_num == 11) {
        int binary_tree_num;

        printf("chose tree:");
        scanf("%d", &binary_tree_num);
        node = BSTreeNodeInit(binary_tree_num);
        PrintTree(node);
        printf("\n");

        height = get_height(node);
        printf("this tree's height is:%d\n", height);

        case011(node);
    } else if (case_num == 13) {
        int backward_num;
        struct ListNode *node;

        printf("chose backward_num:");
        scanf("%d", &backward_num);
        node = case013(backward_num);
        printf("get val:%d\n", node->m_nKey);
    }

    return 0;
}
