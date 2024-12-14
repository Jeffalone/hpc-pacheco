#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char *argv[]) {
  long long int tosses_in_circle, toss = 0;
  long long int total_tosses;
	clock_t start, end; 
  double x, y, dist_squared;
  double pi;
	
  printf("Enter Total Tosses: ");
  scanf("%lld", &total_tosses);
  srand(256);
	start = clock();	
  for (toss = 0; toss < total_tosses; toss++) {
    x = (rand() / (double)RAND_MAX) * 2 - 1;
    y = (rand() / (double)RAND_MAX) * 2 - 1;
    dist_squared = (x * x) + (y * y);
    if (dist_squared <= 1) {
      tosses_in_circle++;
    }
  }
	end = clock();
  pi = 4 * tosses_in_circle / (double)total_tosses;
  printf("Estimate for pi: %f", pi);
	float total_time = (float) (end-start)/ CLOCKS_PER_SEC;
	printf("Estimation took: %f s\n", total_time);
  return EXIT_SUCCESS;
}
