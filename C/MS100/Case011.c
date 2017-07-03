#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

int get_height(struct BSTreeNode *root)
{
    int height = 0;
    int left_height, right_height;
    
    if (root == NULL) {
        return 0;
    }

    left_height = get_height(root->left) + 1;
    right_height = get_height(root->right) + 1;

    height = (left_height > right_height) ? left_height:right_height;

    return height;
}

int get_max(int left, int right)
{
    return ((left > right) ? left:right);
}

int get_max_distance(struct BSTreeNode *root)
{
    int distance_left = 0;
    int distance_right = 0;
    int distance = 0;
    int max;
    
    if (root == NULL) {
        return 0;
    }

    if (root->left) {
        distance_left = get_max_distance(root->left);
    }

    if (root->right) {
        distance_right = get_max_distance(root->right);
    }

    distance = get_height(root->left) + get_height(root->right);
    max = get_max(get_max(distance_left, distance_right), distance);

    return max;
}

void case011(struct BSTreeNode *root)
{
    int max_distance;

    printf("case011===>>>get_max_distance start!\n");

    max_distance = get_max_distance(root);
    
    printf("case011===>>>max distance:%d\n", max_distance);
    printf("case011===>>>get_max_distance end!\n");
}
