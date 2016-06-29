#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int sort_compare(const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

int calc_avg_k_lowest_runs(int **array, int numRums, int k) {
	qsort(*array, numRums, sizeof(int), sort_compare);
	int i;
	for (i = 0; i < numRums; ++i) {
		printf("%d\n", *(*array + i));

	}
}

int main(){
  int *array = malloc(5 * sizeof(int)); 
  int i;
  for (i = 0; i < 5; i++) {
  	array[i] = 5 - i;
  }
  calc_avg_k_lowest_runs(&array, 5, 3);

	
}