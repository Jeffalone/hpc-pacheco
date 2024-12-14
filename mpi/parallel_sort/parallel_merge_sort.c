#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEED_OFFSET 2022
#define TAG 0

// Function Protypes
void Get_list_size(int my_rank, int comm_sz, long long int *size);
void swap(int *a, int *b);
int partition(int *arr, int low, int high);
void quickSort(int *arr, int low, int high);
int *merge(int *arr1, int arr1_sz, int *arr2, int arr2_sz);
void seed_data(int rank, int *arr, long long int size);
int *treeMerge(int rank, int comm_sz, int *my_data, int size);
// Compare function for stdlib.h qsort implementation
int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int main(void) {
  int comm_sz, my_rank;
  int *my_data;
  long long int total_size, my_size;
  double start_time, end_time, my_difference;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  Get_list_size(my_rank, comm_sz, &total_size);
  my_size = total_size / comm_sz;
  if (total_size % comm_sz != 0 && my_rank <= (total_size % comm_sz) - 1) {
    // total_size % comm_sz will always be < comm_sz.
    // Therefore we can distrubute the remainder equally to the remaining
    // processes.
    my_size++;
  }
  my_data = (int *)malloc(sizeof(int) * (int)my_size);
  if (my_data == NULL)
    MPI_Abort(MPI_COMM_WORLD, 1); // malloc error. needs to quit.

  seed_data(my_rank, my_data, my_size);
  // Time local sort and merge only.
  start_time = MPI_Wtime();
  qsort(my_data, my_size, sizeof(int), compare);
  my_data = treeMerge(my_rank, comm_sz, my_data, my_size);
  end_time = MPI_Wtime();
  my_difference = end_time - start_time;

  if (my_data != NULL) {
    printf("Sorted Data from Rank %d:\n", my_rank);
    for (int i = 0; i < total_size; i++) {
      printf("%d, ", my_data[i]);
      if ((i+1) % 10 == 0) {
        // Formatting only 10 numbers per line.
        printf("\n");
      }
    }
    printf("\n");
    printf("Using %d nodes took %fs\n", comm_sz, my_difference);
    free(my_data);
  }
  MPI_Finalize();
  return 0;
}

void Get_list_size(int my_rank, int comm_sz, long long int *size) {
  if (my_rank == 0) {
    printf("Enter final size of sorted data: ");
    scanf("%lld", size);
  }
  MPI_Bcast(size, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
}

void seed_data(int rank, int *arr, long long int size) {
  srand(SEED_OFFSET + rank); // set seed predictably
  for (int i = 0; i < size; i++) {
    arr[i] = rand();
  }
}

int nearest_pow_two(int i) {
  int res = 1;
  while (res < i)
    res <<= 2;
  return res;
}

int *treeMerge(int my_rank, int comm_sz, int *my_data, int my_size) {
  if (comm_sz <= 0) {
    perror("Invalid call to treeMerge(). comm_sz must be a non-zero positive "
           "integer.");
    MPI_Abort(MPI_COMM_WORLD, 1); // invalid call
  }
  if (comm_sz == 1) // Already Merged if there is only one Node
    return my_data;
  int limit;
  if ((comm_sz & (comm_sz - 1)) == 0) {
    // check if is a power of two if not "make dummy nodes"
    limit = comm_sz;
  } else {
    limit = nearest_pow_two(comm_sz);
  }
  int mask = 1;
  int check;
  int pair;

  while (mask < limit) {
    check = my_rank & mask;
    pair = my_rank ^ mask;
    if (pair > comm_sz - 1) {
      // trying a dummy node go on.
      mask = mask << 1;
      continue;
    }

    if (check == mask) {
      // once a process sends data it is finished participating in the
      // tree-structured merge.
      MPI_Send(my_data, my_size, MPI_INT, pair, TAG, MPI_COMM_WORLD);
      free(my_data);
      return NULL;
    }

    if (check == 0) {
      MPI_Status status;
      int *pair_msg;
      int pair_msg_size;

      // probe for pair message, and determine size. This allows for two
      // different sizes of data.
      MPI_Probe(pair, TAG, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_INT, &pair_msg_size);

      // Allocate enough space for incoming message.
      pair_msg = (int *)malloc(sizeof(int) * (int)pair_msg_size);
      MPI_Recv(pair_msg, pair_msg_size, MPI_INT, pair, TAG, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
			// Uncomment below for debugging
      //printf("Rank %d recieved %d ints from %d\n", my_rank, pair_msg_size, pair);
      my_data = merge(my_data, my_size, pair_msg, pair_msg_size);
      my_size = my_size + pair_msg_size;
    }
    mask = mask << 1;
  }
  if (my_data == NULL || my_rank != 0) {
    // Data should be garunteed not be be null.
    // This line should only be reached by process 0.
    perror("Critical error in treeMerge");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  return my_data;
}

int *merge(int *arr1, int arr1_sz, int *arr2, int arr2_sz) {
  // This function will free arr1 and arr2.
  int new_size = arr1_sz + arr2_sz;
  int *mergedArr = (int *)malloc(sizeof(int) * new_size);
  int i, j, k;
  i = j = k = 0;

  while (i < arr1_sz && j < arr2_sz) {
    if (arr1[i] < arr2[j]) {
      mergedArr[k++] = arr1[i++];
    } else {
      mergedArr[k++] = arr2[j++];
    }
  }
  while (i < arr1_sz)
    mergedArr[k++] = arr1[i++];
  while (j < arr2_sz)
    mergedArr[k++] = arr2[j++];

  free(arr1);
  free(arr2);
  return mergedArr;
}
