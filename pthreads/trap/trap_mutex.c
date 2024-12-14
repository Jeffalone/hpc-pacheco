#include "timer.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 32

void get_input(int *threads, int *n, double *a, double *b) {
  printf("Enter number of threads to use: ");
  scanf("%d", threads);
  if (*threads > MAX_THREADS) {
    perror("Thread count exceeds max\n");
    exit(1);
  }

  printf("Enter number of trapezoids to use: ");
  scanf("%d", n);

  printf("Enter interval from a to b as \"a b\": ");
  scanf("%lf %lf", a, b);

  if (*a > *b) {
    perror("a endpoint must be less than b end point\n");
    exit(1);
  }
}

void *Trap(void *rank);

double f(double x) { return x * x; }

int global_n;
double global_a, global_b, global_h, global_est;
double final_time;
pthread_mutex_t mutex;
int thread_count;

int main(int argc, char *argv[]) {
  pthread_t *threads;
  long thread;
  double start, end;
  get_input(&thread_count, &global_n, &global_a, &global_b);

  global_h = (global_b - global_a) / global_n;
  threads = malloc(thread_count * sizeof(pthread_t));
  pthread_mutex_init(&mutex, NULL);

  global_est = 0.0;
  GET_TIME(start);
  for (thread = 0; thread < thread_count; thread++) {
    pthread_create(&threads[thread], NULL, Trap, (void *)thread);
  }

  for (thread = 0; thread < thread_count; thread++) {
    pthread_join(threads[thread], NULL);
  }
  GET_TIME(end);
  final_time = end - start;
  printf(
      "Estimate with %d trapezoids: %lf\nTook %lf seconds\nWith %d threads\n",
      global_n, global_est, final_time, thread_count);
	pthread_mutex_destroy(&mutex);
  return 0;
}

void print_timing(int mode) {}

void *Trap(void *rank) {
  long my_rank = (long)rank;
  double my_a, my_b, x, estimate;
  int my_n, i;

  my_n = global_n / thread_count;
  my_a = global_a + my_rank * my_n * global_h;
  my_b = my_a + my_n * global_h;

  // estimate locally
  estimate = (f(my_a) + f(my_b)) / 2.0;
  for (i = 1; i <= my_n; i++) {
    x = my_a + i * global_h;
    estimate += f(x);
  }
  estimate *= global_h;

  // mutex wait to update
  pthread_mutex_lock(&mutex);
  printf("Thread %ld done mutex waiting\n", my_rank);
  global_est += estimate;
  pthread_mutex_unlock(&mutex);
  return NULL;
}
