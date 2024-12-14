#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void get_input_size(int my_rank, int comm_sz, int *n);
void get_inputs(int my_rank, int comm_sz, int n, int local_n, int *scalar,
                double vec_a[], double vec_b[]);
void dot_product(int my_rank, double vec_a[], double vec_b[], int local_n);
void scalar_mult(int my_rank, int n, double vec_a[], double vec_b[], int local_n, int scalar);

int main(int argc, char *argv[]) {
  int my_rank, comm_sz, n, local_n, scalar;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  get_input_size(my_rank, comm_sz, &n);
  local_n = n / comm_sz;
  double vec_a[local_n], vec_b[local_n];

  get_inputs(my_rank, comm_sz, n, local_n, &scalar, vec_a, vec_b);
  dot_product(my_rank, vec_a, vec_b, local_n);
	scalar_mult(my_rank, n, vec_a, vec_b, local_n, scalar);

  MPI_Finalize();
  return 0;
}

void get_input_size(int my_rank, int comm_sz, int *n) {
  if (my_rank == 0) {
    printf("Enter the size of vectors you wish to multiply: ");
    scanf("%d", n);
    while (*n % comm_sz != 0 || *n < comm_sz) {
      printf("Invalid size for vectors.\nPlease input a size >= and "
             "evenly divisible by %d: ",
             comm_sz);
      scanf("%d", n);
    }
  }
  MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void get_inputs(int my_rank, int comm_sz, int n, int local_n, int *scalar,
                double vec_a[], double vec_b[]) {
  double *vec1 = NULL;
  double *vec2 = NULL;
  if (my_rank == 0) {

    vec1 = (double *)malloc(sizeof(double) * n);
    vec2 = (double *)malloc(sizeof(double) * n);

    printf(
        "Please input %d components of the first vector separated by spaces: ",
        n);
    for (int i = 0; i < n; i++) {
      scanf("%lf", &vec1[i]);
    }

    printf(
        "Please input %d components of the second vector separated by spaces: ",
        n);
    for (int i = 0; i < n; i++) {
      scanf("%lf", &vec2[i]);
    }

    printf("Please input the scalar you would like to multiply the "
           "vectors by: ");
    scanf("%d", scalar);

    MPI_Bcast(scalar, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec1, local_n, MPI_DOUBLE, vec_a, local_n, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);
    MPI_Scatter(vec2, local_n, MPI_DOUBLE, vec_b, local_n, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);

    free(vec1);
    free(vec2);
  } else {
    MPI_Bcast(scalar, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec1, local_n, MPI_DOUBLE, vec_a, local_n, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);
    MPI_Scatter(vec2, local_n, MPI_DOUBLE, vec_b, local_n, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);
  }
}

void dot_product(int my_rank, double vec_a[], double vec_b[], int local_n) {
	double global_sum = 0;
	double local_sum = 0;
	for(int i = 0; i < local_n; i++){
		local_sum += vec_a[i] * vec_b[i];
	}
	if (my_rank == 0) {
		MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		printf("Result of dot product between first and second vector: %lf\n", global_sum);
	}else {
		MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	}
}
void scalar_mult(int my_rank,int n, double vec_a[], double vec_b[], int local_n, int scalar){
	double res_a[local_n], res_b[local_n];
	for(int i = 0; i < local_n; i++){
		res_a[i] = vec_a[i] * scalar;
		res_b[i] = vec_b[i] * scalar;
	}

	double *global_a = NULL;
	double *global_b = NULL;

	if (my_rank == 0){
		global_a = malloc(sizeof(double) * n);
		global_b = malloc(sizeof(double) * n);

		MPI_Gather(res_a, local_n, MPI_DOUBLE, global_a, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Gather(res_b, local_n, MPI_DOUBLE, global_b, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		printf("Result of the first vector * %d: ", scalar);
		for(int i = 0; i < n; i++){
			printf("%lf ", global_a[i]);
		}
		printf("\n");

		printf("Result of the second vector * %d: ", scalar);
		for(int i = 0; i < n; i++){
			printf("%lf ", global_b[i]);
		}
		printf("\n");
		free(global_b);
		free(global_a);
	}else {
		MPI_Gather(res_a, local_n, MPI_DOUBLE, global_a, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Gather(res_b, local_n, MPI_DOUBLE, global_b, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}

}
