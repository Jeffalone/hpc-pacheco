#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEED_OFFSET 256

void Get_num_tosses(int my_rank, int comm_sz, long long int *tosses) {
  if (my_rank == 0) {
    printf("Enter in number of tosses: ");
    scanf("%lld", tosses);
  }
  // does not contine until all processes hit this line and data is sent.
  MPI_Bcast(tosses, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
}

long long int toss(long long int toss_count, int my_rank) {
  long long int in_circle = 0;
  double x, y, dist_squared;
  srand(SEED_OFFSET + my_rank); // predictably set random seed.
  for (long long int curr_toss = 0; curr_toss < toss_count; curr_toss++) {
    x = (rand() / (double)RAND_MAX) * 2 - 1;
    y = (rand() / (double)RAND_MAX) * 2 - 1;
    dist_squared = (x * x) + (y * y);
    if (dist_squared <= 1) {
      in_circle++;
    }
  }
  return in_circle;
}

int main(void) {
  int comm_sz, my_rank;
  long long int total_tosses, my_num_tosses, my_toss_in_circle,
      global_in_circle;
  double start_time, end_time, my_difference, global_min_time;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  Get_num_tosses(my_rank, comm_sz, &total_tosses);

  my_num_tosses = total_tosses / comm_sz;
  if (total_tosses % comm_sz != 0 && my_rank <= (total_tosses % comm_sz) - 1) {
    // total_tosses % comm_sz will always be < comm_sz.
    // Therefore we can distrubute the remainder equally to the remaining
    // processes.
    my_num_tosses++;
  }

  // Only time generation and communication
  start_time = MPI_Wtime();
  my_toss_in_circle = toss(my_num_tosses, my_rank);
  MPI_Reduce(&my_toss_in_circle, &global_in_circle, 1, MPI_LONG_LONG, MPI_SUM,
             0, MPI_COMM_WORLD);
  end_time = MPI_Wtime();

  // We take the minimum time that a node took to complete the task as
  // recommended in chapter Two.
  my_difference = end_time - start_time;
  MPI_Reduce(&my_difference, &global_min_time, 1, MPI_DOUBLE, MPI_MIN, 0,
             MPI_COMM_WORLD);
  if (my_rank == 0) {
    double pi;
    pi = 4 * global_in_circle / (double)total_tosses;
    printf("Estimate for pi: %f\n", pi);
    printf("Using %d nodes took %fs\n", comm_sz, global_min_time);
  }

  MPI_Finalize();
  return 0;
}
