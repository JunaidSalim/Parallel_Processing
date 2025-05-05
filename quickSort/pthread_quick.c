#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "utils.h"

#define NUM_THREADS 8
#define THRESHOLD 1000

typedef struct {
    int* arr;
    int low;
    int high;
} SortArgs;

void quickSort(int* arr, int low, int high) {
    if (low < high) {
        int pivot = medianOfThree(arr, low, high);
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void* parallelQuickSort(void* arg) {
    SortArgs* args = (SortArgs*)arg;
    int low = args->low;
    int high = args->high;
    
    if (low < high) {
        if (high - low > THRESHOLD) {
            int pivot = medianOfThree(args->arr, low, high);
            int pi = partition(args->arr, low, high);
            
            pthread_t tid;
            SortArgs leftArgs = {args->arr, low, pi - 1};
            pthread_create(&tid, NULL, parallelQuickSort, &leftArgs);
            
            SortArgs rightArgs = {args->arr, pi + 1, high};
            parallelQuickSort(&rightArgs);
            
            pthread_join(tid, NULL);
        } else {
            quickSort(args->arr, low, high);
        }
    }
    return NULL;
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
    SortArgs args = {arr, 0, size - 1};
    parallelQuickSort(&args);
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