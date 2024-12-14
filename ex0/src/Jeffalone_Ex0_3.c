#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long int time_diff(struct timespec *start, struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000000000 +
         (end->tv_nsec - start->tv_nsec);
}
int main(int argc, char *argv[]) {
  char *fname = "./mv.txt";
  int temp, numrows, numcols, vrows;
  FILE *fhandle;

  if (argc == 2) {
    fname = argv[1];
  }

  fhandle = fopen(fname, "r");
  if (fhandle == NULL) {
    perror("Error reading in file");
    exit(1);
  }
  // read in matrix dimensions
  fscanf(fhandle, "%d %d", &numrows, &numcols);
  printf("numrows: %d\nnumcols: %d\n", numrows, numcols);

  // read in matrix.
  int matrix[numrows][numcols];
  for (int i = 0; i < numrows; i++) {
    for (int j = 0; j < numcols; j++) {
      fscanf(fhandle, "%d", &temp);
      matrix[i][j] = temp;
    }
  }
  // read in vector
  fscanf(fhandle, "%d", &vrows);
  int vector[vrows];
  for (int i = 0; i < vrows; i++) {
    fscanf(fhandle, "%d", &temp);
    vector[i] = temp;
  }

  if (numcols != vrows) {
    perror("Mismatch of matrix columns and vector rows");
    exit(1);
  }
  // prepare result and multiply
  int result[vrows];
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < numrows; i++) {
    temp = 0;
    for (int j = 0; j < numcols; j++) {
      temp += matrix[i][j] * vector[j];
    }
    result[i] = temp;
  }
  clock_gettime(CLOCK_MONOTONIC, &end);

  for (int i = 0; i < vrows; i++) {
    printf("%d\n", result[i]);
  }
  long long int elapsed_time = time_diff(&start, &end);
  printf("Time took: %lld\n", elapsed_time);
  fclose(fhandle);
  return 0;
}
