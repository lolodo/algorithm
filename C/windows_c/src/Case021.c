/*
编程求解：
输入两个整数n 和m，从数列1，2，3.......n 中随意取几个数,
使其和等于m ,要求将其中所有的可能组合列出来.
ANSWER
This is a combination generation problem. 
void findCombination(int n, int m) {
  if (n>m) findCombination(m, m);
  int aux[n];
  memset(aux, 0, n*sizeof(int));
  helper(m, 0, aux);
}
void helper(int dest, int idx, int aux[], int n) {
  if (dest == 0) 
    dump(aux, n);
  if (dest <= 0 || idx==n) return;
  helper(dest, idx+1, aux, n);
  aux[idx] = 1;
  helper(dest-idx-1, idx+1, aux, n);
  aux[idx] = 0;
}
void dump(int aux[], int n) {
  for (int i=0; i<n; i++) 
    if (aux[i]) printf(“%3d”, i+1);
  printf(“\n”);
}
PS: this is not an elegant implementation, however, it is not necessary to use gray code or other techniques for such a problem, right?
*/

/*
1～n， sum，那么如果n > sum，那么大于sum的那些部分肯定用不上，因此可以去掉；于是前提一定是n 小于或者等于sum。
假设f(sum, n)为在1～n中随意取几个数后，其和为m的解，那么应该有：
 f(sum, n) = f (sum - n, n - 1) + f(sum, n - 1)
 也就是说分两种情况，取n，然后在1～n-1数列中找出其和为sum - n的方法；如果不取n，那么其实是从1～n-1中取其和为sum的方法。
*/

#include <stdio.h>
#include <stdlib.h>

void print_result(int *array, int end)
{
  int i = 0;

  while (i < end)
  {
    if (array[i])
    {
      printf("%d ", i);
    }
    i++;
  }
}

void find_combination(int n, int sum, int *array)
{
  int result;

  if (n > sum)
  {
    return find_combination(sum, sum, array);
  }

  result = ((n + 1) * n) >> 1;
  if (result < sum)
  {
    return;
  }

  if (sum == 0)
  {
    print_result(array, 100);
    printf("\n");
    return;
  }

  if (n < 1)
  {
    return;
  }

  array[n] = 1;
  find_combination(n - 1, sum - n, array);

  array[n] = 0;
  find_combination(n - 1, sum, array);
}

int main(void)
{
  int n, sum;
  int array[100] = {0};

  printf("Please input n:");
  scanf("%d", &n);
  printf("Please input sum:");
  scanf("%d", &sum);

  find_combination(n, sum, array);

  return 0;
}