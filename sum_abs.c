// 题目2：算法题，只需要写清思路即可 （要求答案的复杂度为o（nlogn））
// 给定一个数组，里面的数字有正有负。求该数组中和的绝对值最小的连续子数组。

// 例：
// 1 3 -2 -5 1 6 7
// 答案为 0， 子数组为 【-2 -5 1 6】

// ===============================
#include <stdio.h>
#include <stdlib.h>

int array[] = {1, 3, -2, -5, 1, 6, 7};

int partition(int *src, int left, int right)
{
    int start = left, end = right;
    int tmp = 0;

    left++;
    while (left < right)
    {
        while (left <= end && src[left] <= src[start])
        {
            left++;
        }

        while (src[right] > src[start])
        {
            right--;
        }

        if (left < right)
        {
            tmp = src[left];
            src[left] = src[right];
            src[right] = tmp;
        }
    }

    tmp = src[start];
    src[start] = src[right];
    src[right] = tmp;

    return right;
}

void quick_sort(int *src, int left, int right)
{
    if (left < right)
    {
        int mid = partition(src, left, right);
        quick_sort(src, left, mid - 1);
        quick_sort(src, mid + 1, right);
    }
}

int min_abs_sum(int *input, int len, int *info)
{
    int *sum, all = 0;
    int i, len_count;
    int from_zero = 0;
    int cur, val, start, end, tmp;

    if (!input)
    {
        printf("input is null!\n");
        return -1;
    }

    if (!info)
    {
        printf("ouput is null!\n");
        return -1;
    }

    if (len <= 0)
    {
        printf("input length is error!\n");
        return -1;
    }

    sum = malloc(len * sizeof(int));
    for (i = 0; i < len; i++)
    {
        all += *(input + i);
        *(sum + i) = all;
        *(input + i) = all;
    }

    quick_sort(sum, 0, len - 1);
    printf("sum:");
    for (i = 0; i < len; i++)
    {
        printf("%d\t", sum[i]);
    }
    printf("\n");

    // cur = info;
    // start = info + 1;
    // end = info + 2;
    len_count = len - 2;
    val = abs(sum[len_count + 1]);
    from_zero = 1;
    start = 0;
    end = sum[len_count + 1];

    while (len_count)
    {
        cur = abs(sum[len_count + 1] - sum[len_count]);
        // printf("The first compare %dth and %dth:%d, %d\n", len_count + 1, len_count, cur, val);
        if (cur < val)
        {
            val = cur;
            from_zero = 0;
            start = sum[len_count];
            end = sum[len_count + 1];
        }

        cur = abs(sum[len_count]);
        // printf("The second compare %dth:%d, %d\n", len_count, cur, val);
        if (cur < val)
        {
            val = cur;
            from_zero = 1;
            start = 0;
            end = sum[len_count];
        }
        len_count--;
    }

    //Get the right direction
    if (!from_zero)
    {
        for (i = 0; i < len; i++)
        {
            if (input[i] == start)
            {
                start = i;
                break;
            }
        }
    }

    len_count = len;
    while (len_count--)
    {
        if (input[len_count] == end)
        {
            end = len_count;
            break;
        }
    }

    if (start < end)
    {
        start += 1;
    }
    else
    {
        cur = start;
        start = end + 1;
        end = cur;
    }

    *(info) = val;
    *(info + 1) = start;
    *(info + 2) = end;

    free(sum);

    return 0;
}

int main(void)
{
    int i, output[3], len, array_len;
    int ret;

    array_len = sizeof(array) / sizeof(array[0]);
    printf("origin:");
    for (i = 0; i < array_len; i++)
    {
        printf("%d\t", array[i]);
    }
    printf("\n");

    ret = min_abs_sum(array, array_len, output);
    if (ret)
    {
        printf("Nof found!\n");
    }
    else
    {
        printf("The smallest value is %d, from %dth to %dth unit.\n", output[0], output[1], output[2]);
    }

    while (array_len--)
    {
        array[array_len] = array[array_len] - array[array_len - 1];
    }

    printf("result:");
    for (i = output[1]; i < output[2] + 1; i++)
    {
        printf("%d\t", array[i]);
    }
    printf("\n");

    return 0;
}