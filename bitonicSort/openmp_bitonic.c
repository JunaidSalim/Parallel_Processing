#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define THRESHOLD 1000

void parallelBitonicSort(int* arr, int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        
        if (cnt > THRESHOLD) {
            #pragma omp task shared(arr)
            parallelBitonicSort(arr, low, k, 1);
            
            #pragma omp task shared(arr)
            parallelBitonicSort(arr, low + k, k, 0);
            
            #pragma omp taskwait
            bitonicMerge(arr, low, cnt, dir);
        } else {
            bitonicSort(arr, low, cnt, dir);
        }
    }
}

void bitonicSort(int* arr, int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        bitonicSort(arr, low, k, 1);
        bitonicSort(arr, low + k, k, 0);
        bitonicMerge(arr, low, cnt, dir);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <arraySize>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    if ((size & (size - 1)) != 0) {
        printf("Array size must be power of 2\n");
        return 1;
    }

    int* arr = malloc(size * sizeof(int));
    
    int numThreads = omp_get_num_procs();
    omp_set_num_threads(numThreads);
    
    srand(time(NULL));
    generateRandomArray(arr, size);
    
    printf("Using %d OpenMP threads\n", numThreads);
    printf("First few elements before sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");

    double start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        parallelBitonicSort(arr, 0, size, 1);
    }
    double end = omp_get_wtime();
    
    printf("First few elements after sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");
    
    printf("Array is sorted: %s\n", isSorted(arr, size) ? "Yes" : "No");
    printf("Time taken: %f seconds\n", end - start);
    
    free(arr);
    return 0;
}