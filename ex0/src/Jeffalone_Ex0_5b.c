#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  int i, j;
  int n = 128;
  double sum;
  clock_t end, start;
  if (argc == 2) {
    int multiplier = atoi(argv[1]);
    for (int i = 0; i < multiplier; i++) {
      n *= 2;
    }
  }

  printf("Array Size: %d\n", n);
  double arr[n][n];

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      arr[i][j] = (double)rand() / RAND_MAX;
    }
  }

  sum = 0;
  start = clock();
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      sum += arr[i][j];
    }
  }
  end = clock();
  printf("Row Major: sum = %lf and Clock Ticks are %ld\n", sum, end - start);

  sum = 0;
  start = clock();
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      sum += arr[j][i];
    }
  }
  end = clock();
  printf("Column Major: sum = %lf and Clock Ticks are %ld\n", sum, end - start);
  return 0;
}
