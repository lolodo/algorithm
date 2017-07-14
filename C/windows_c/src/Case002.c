/*
2.设计包含min 函数的栈。
定义栈的数据结构，要求添加一个min 函数，能够得到栈的最小元素。
要求函数min、push 以及pop 的时间复杂度都是O(1)。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MinStackElement
{
    int data;
    int min;
};

struct MinStack
{
    struct MinStackElement *data;
    int size;
    int top;
    // int min;
};

struct MinStack stack;

void MinStackInit(int maxSize)
{
    stack.size = maxSize;
    stack.data = (struct MinStackElement *)malloc(sizeof(struct MinStackElement) * maxSize);
    memset(stack.data, 0, sizeof(struct MinStackElement) * maxSize);
    stack.top = 0;
    // stack.min = 0;
}

void MinStackFree(struct MinStack *stack)
{
    free(stack->data);
}

void MinStackPush(struct MinStack *stack, int d)
{
    struct MinStackElement *p = stack->data + stack->top;

    if (stack->top == stack->size)
    {
        printf("the stack if full!\n");
        return;
    }

    p->data = d;
    p->min = (stack->top == 0) ? d : stack->data[stack->top - 1].data;
    if (p->min > d)
    {
        p->min = d;
    }

    stack->top++;
}

int MinStackPop(struct MinStack *stack, int *val)
{
    if (!stack)
    {
        return -1;
    }

    if (stack->top == 0)
    {
        printf("the stack is empty!\n");
        return -1;
    }

    *val = stack->data[stack->top - 1].data;
    stack->top--;

    return 0;
}

int MinStackIsEmpty(struct MinStack *stack)
{
    if (stack->top)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int MinStackMin(struct MinStack *stack, int *min)
{
    // int val;

    if (!stack)
    {
        printf("input error!\n");
        return -1;
    }

    if (stack->top == 0)
    {
        printf("stack is empty!\n");
        return -1;
    }

    *min = stack->data[stack->top - 1].data;
    return 0;
}

void PrintStack(struct MinStack *stack)
{
    int i;

    if (!stack)
    {
        return;
    }

    if(stack->top == 0) {
        return;
    }

    for (i = 0; i < stack->top; i++)
    {
        printf("val:%d min:%d\n", stack->data[i].data, stack->data[i].min);
    }

    printf("end!\n");
}
int main(int argc, char *args[])
{
    int val = -1;
    int ret;

    MinStackInit(20);

    MinStackPush(&stack, 10);
    MinStackPush(&stack, 109);
    MinStackPush(&stack, 4);
    PrintStack(&stack);
    while (!MinStackIsEmpty(&stack))
    {
        ret = MinStackPop(&stack, &val);
        if (ret)
        {
            printf("error happend!\n");
            return -1;
        }

        PrintStack(&stack);
    }

    MinStackFree(&stack);
}