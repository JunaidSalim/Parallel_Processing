#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define THRESHOLD 1000

void quickSort(int* arr, int low, int high) {
    if (low < high) {
        int pivot = medianOfThree(arr, low, high);
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void parallelQuickSort(int* arr, int low, int high) {
    if (low < high) {
        if (high - low > THRESHOLD) {
            int pivot = medianOfThree(arr, low, high);
            int pi = partition(arr, low, high);
            
            #pragma omp task shared(arr)
            parallelQuickSort(arr, low, pi - 1);
            
            #pragma omp task shared(arr)
            parallelQuickSort(arr, pi + 1, high);
            
            #pragma omp taskwait
        } else {
            quickSort(arr, low, high);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <arraySize>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
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
        parallelQuickSort(arr, 0, size - 1);
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