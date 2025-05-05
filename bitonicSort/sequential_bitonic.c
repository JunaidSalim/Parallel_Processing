#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

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
    srand(time(NULL));
    generateRandomArray(arr, size);
    
    printf("First few elements before sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");

    clock_t start = clock();
    bitonicSort(arr, 0, size, 1);
    clock_t end = clock();
    
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("First few elements after sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");
    
    printf("Array is sorted: %s\n", isSorted(arr, size) ? "Yes" : "No");
    printf("Time taken: %f seconds\n", timeSpent);
    
    free(arr);
    return 0;
}