#include <stdio.h>

#define N 4

int matrix[N][N] = {{0, 0, 1, 0},
                    {0, 0, 1, 0},
                    {0, 0, 0, 0},
                    {0, 0, 1, 0}};

int knows(int a, int b)
{
    return matrix[a][b];
}

int main()
{
    int i, j;

    // for (i = 0; i < 4; i ++) {
    //     for (j = 0; j < 4; j++) {
    //         if (matrix[i][j]) {
    //             printf("The %dth man knows the %dth man\n", i, j);
    //         } else {
    //             printf("The %dth man doesn't know the %dth man\n", i, j);
    //         }
    //     }
    // }
    i = 0; 
    j = 1;

    while ((i < N) || (j < N))
    {
        if (knows(i, j))
        {
            i++;
            continue;
        }
        else
        {
            j++;
            if (j == N) {
                printf("The %dth people is the celebrity!\n", i+1);
                break;
            }
        }
    }

    return 0;
}