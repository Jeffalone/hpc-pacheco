#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RUNS 10000
#define SEED 12345

enum operation { MULT, DIV, SQRT, SINE };

long long int time_diff(struct timespec *start, struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000000000 +
         (end->tv_nsec - start->tv_nsec);
}

int main(int argc, char *argv[]) {
  int run = 0;
  long timings[4];
  struct timespec start, end;
  int L, R;
  double S;

  srand(SEED); // seed for consistent test set
  for (run; run < RUNS; run++) {
    L = rand();
    R = rand();
    clock_gettime(CLOCK_MONOTONIC, &start);
    L *R;
    clock_gettime(CLOCK_MONOTONIC, &end);
    timings[MULT] += time_diff(&start, &end);
  }

  srand(SEED); // re-seed for consistent test set
  for (run; run < RUNS; run++) {
    L = rand();
    R = rand();
    clock_gettime(CLOCK_MONOTONIC, &start);
    L / R;
    clock_gettime(CLOCK_MONOTONIC, &end);
    timings[DIV] += time_diff(&start, &end);
  }

  srand(SEED);
  for (run; run < RUNS; run++) {
    L = rand();
    clock_gettime(CLOCK_MONOTONIC, &start);
    sqrt(L);
    clock_gettime(CLOCK_MONOTONIC, &end);
    timings[SQRT] += time_diff(&start, &end);
  }

  srand(SEED);
  for (run; run < RUNS; run++) {
    S = (double)rand();
    clock_gettime(CLOCK_MONOTONIC, &start);
    sin(L);
    clock_gettime(CLOCK_MONOTONIC, &end);
    timings[SINE] += time_diff(&start, &end);
  }

  printf("Timings (ns):\nMult: %f\nDiv: %f\nSqrt: %f\nSine: %f\n",
         timings[MULT] * 1.0 / RUNS, timings[DIV] * 1.0 / RUNS,
         timings[SQRT] * 1.0 / RUNS, timings[SINE] * 1.0 / RUNS);
}
