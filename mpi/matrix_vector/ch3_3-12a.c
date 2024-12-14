#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define RUNS 100000
#define SEED 1000

void ring_sum(int my_rank, int comm_sz, int* input, int* output);

int main(int argc, char *argv[]) {
  int my_rank, comm_sz, i;
	double start, end, ring_avg, reduce_avg;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	srand(SEED + my_rank);
	int my_val;
	ring_avg = reduce_avg = 0.0;
	for(i = 0; i < RUNS; i++){
		int res = 0;
		my_val = rand();
		start = MPI_Wtime();
		// processes should syncronize here. Hopefully doesn't get out of sync.
		MPI_Allreduce(&my_val, &res,1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		end = MPI_Wtime();
		reduce_avg += (end - start) / RUNS;
		
		start = MPI_Wtime();
		ring_sum(my_rank, comm_sz, &my_val, &res);
		end = MPI_Wtime();
		ring_avg += (end - start) / RUNS;
	}
	double local_averages[2] = { reduce_avg, ring_avg };
	double global_averages[2];

	MPI_Reduce(local_averages, global_averages, 2, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

	if(my_rank == 0){
		printf("MPI_Allreduce min avg time: %.7lf\nRing Pass min avg time:%.7lf\n", global_averages[0], global_averages[1]);
	}

  MPI_Finalize();
  return 0;
}

void ring_sum(int my_rank, int comm_sz, int* input, int* output){
	int temp;	
	temp = *input;
	*output = *input;
	int dest = (my_rank+1)%comm_sz;
	int source = my_rank == 0 ? comm_sz -1 : (my_rank-1);
	
	for(int i = 1; i < comm_sz; i++){
		MPI_Sendrecv_replace(&temp, 1, MPI_INT, dest, 0, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		*output += temp;
	}
}
