/*
第9 题
判断整数序列是不是二元查找树的后序遍历结果
题目：输入一个整数数组，判断该数组是不是某二元查找树的后序遍历的结果。
如果是返回true，否则返回false。
例如输入5、7、6、9、11、10、8，由于这一整数序列是如下树的后序遍历结果：
   8
  /  \
 6    10
/ \   / \
5 7   9 11
因此返回true。
如果输入7、4、6、5，没有哪棵树的后序遍历的结果是这个序列，因此返回false。
*/
/*
    后序遍历结果数组的最后一个元素为根节点
    二元查找树的性质：根节点的左子树全部小于根节点， 右子树全部大于根节点，
*/

#include <stdio.h>

int array0[] = {5, 7, 6, 9, 11, 10, 8};
int array1[] = {5, 7, 6, 9, 11, 8, 10};

int helper(int *a, int start, int end)
{
    int pivot;
    int i, left_end;

    if (!a) {
        return 0;
    }

    if (start == end) {
        return 1;
    }

    if (end <= 0) {
        return 0;
    }
    
    i = end;
    pivot = a[i];
    i--;

    while(pivot < a[i] && i >= start) {
        i--;
    }
    if(!helper(a, i + 1, end - 1)) {
        return 0;
    }

    left_end = i;
    while(pivot > a[i] && i >= start) {
        i--;
    }
    if(!helper(a, i + 1, left_end)) {
        return 0;
    } else {
        return 1;
    }

    if(i + 1 > start) {
        return 0;
    }
}

int main(int argc, char *args[])
{
    int ret;

    ret = helper(array1, 0, 6);
    if (ret) {
        printf("true\n");
    } else {
        printf("false\n");
    }

    return 0;
}

