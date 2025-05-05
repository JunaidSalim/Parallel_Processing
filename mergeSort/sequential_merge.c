#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

void mergeSort(int* arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <arraySize>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    int* arr = malloc(size * sizeof(int));
    
    srand(time(NULL));
    generateRandomArray(arr, size);
    
    printf("First few elements before sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");

    clock_t start = clock();
    mergeSort(arr, 0, size - 1);
    clock_t end = clock();
    
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("First few elements after sorting: ");
    for(int i = 0; i < (size < 10 ? size : 10); i++) printf("%d ", arr[i]);
    printf("\n");
    
    printf("Array is sorted: %s\n", isSorted(arr, size) ? "Yes" : "No");
    printf("Time taken: %f seconds\n", time_spent);
    
    free(arr);
    return 0;
}