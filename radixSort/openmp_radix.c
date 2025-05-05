#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"

#define THRESHOLD 1000

void parallelCountSort(int* arr, int size, int exp) {
    int* output = malloc(size * sizeof(int));
    int count[10] = {0};
    
    #pragma omp parallel
    {
        int localCount[10] = {0};
        
        #pragma omp for
        for (int i = 0; i < size; i++)
            localCount[(arr[i] / exp) % 10]++;
        
        #pragma omp critical
        for (int i = 0; i < 10; i++)
            count[i] += localCount[i];
    }
    
    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];
    
    #pragma omp parallel for
    for (int i = size - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        int pos;
        #pragma omp atomic capture
        pos = --count[digit];
        output[pos] = arr[i];
    }
    
    #pragma omp parallel for
    for (int i = 0; i < size; i++)
        arr[i] = output[i];
    
    free(output);
}

void parallelRadixSort(int* arr, int size) {
    int max = getMax(arr, size);
    
    int numThreads = omp_get_num_procs();
    omp_set_num_threads(numThreads);
    
    for (int exp = 1; max/exp > 0; exp *= 10) {
        if (size > THRESHOLD)
            parallelCountSort(arr, size, exp);
        else
            countSort(arr, size, exp);
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
    parallelRadixSort(arr, size);
    double end = omp_get_wtime();
    
    printf("First few elements after sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");
    
    printf("Array is sorted: %s\n", isSorted(arr, size) ? "Yes" : "No");
    printf("Time taken: %f seconds\n", end - start);
    
    free(arr);
    return 0;
}