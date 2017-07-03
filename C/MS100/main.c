#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

int main(int argc, char *args[]) 
{
    int i, height;
    struct BSTreeNode *node;

    if (argc < 2) {
        return -1;
    }

    printf("intput %d parameters!\n", argc);
    for (i = 0; i < argc; i++) {
        printf("%dth:%s\n", i, args[i]);
    }

    node = BSTreeNodeInit(atoi(args[1]));
    PrintTree(node);
    printf("\n");

    height = get_height(node);
    printf("this tree's height is:%d\n", height);

    case011(node);

    return 0;
}
