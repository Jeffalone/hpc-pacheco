#include <math.h>
#include <stdio.h>
#include <time.h>

long long int time_diff(struct timespec *start, struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000000000 +
         (end->tv_nsec - start->tv_nsec);
}

void approx_pi(int k, long double *bounds) {
  long double upper_bound = 2 * sqrt(3.0);
  long double lower_bound = 3;

  for (int i = 1; i < k; i++) {
    upper_bound = (2 * upper_bound * lower_bound) / (upper_bound + lower_bound);
    lower_bound = sqrtl(upper_bound * lower_bound);
  }
  bounds[1] = upper_bound;
  bounds[0] = lower_bound;
}

int main(int argc, char *argv[]) {
  struct timespec start;
  struct timespec end;
  long double pi_bounds[2];
  long long int elapsed_time;

  for (int i = 1; i <= 6; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    approx_pi(i, pi_bounds);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = time_diff(&start, &end);

    printf("Sides: %d\n", (int)(3 * pow(2, (double)i)));
    printf("Lower Bound: %.30Lf\n", pi_bounds[0]);
    printf("Upper Bound: %.30Lf\n", pi_bounds[1]);
    printf("Time(ns): %lld\n\n", elapsed_time);
  }
  return 0;
}
