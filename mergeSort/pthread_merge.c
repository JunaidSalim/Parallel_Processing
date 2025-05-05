#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "utils.h"

#define NUM_THREADS 8
#define THRESHOLD 1000

typedef struct {
    int* arr;
    int left;
    int right;
} SortArgs;

void* parallelMergeSort(void* arg) {
    SortArgs* args = (SortArgs*)arg;
    int left = args->left;
    int right = args->right;
    int mid = left + (right - left) / 2;

    if (left < right) {
        if (right - left > THRESHOLD) {
            pthread_t tid;
            SortArgs leftArgs = {args->arr, left, mid};
            pthread_create(&tid, NULL, parallelMergeSort, &leftArgs);
            
            SortArgs rightArgs = {args->arr, mid + 1, right};
            parallelMergeSort(&rightArgs);
            
            pthread_join(tid, NULL);
            merge(args->arr, left, mid, right);
        } else {
            mergeSort(args->arr, left, right);
        }
    }
    return NULL;
}

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
    SortArgs args = {arr, 0, size - 1};
    parallelMergeSort(&args);
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