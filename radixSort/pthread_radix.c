#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "utils.h"

#define NUM_THREADS 8
#define THRESHOLD 1000

typedef struct {
    int* arr;
    int start;
    int end;
    int exp;
} SortArgs;

pthread_barrier_t barrier;

void* parallelCountSort(void* arg) {
    SortArgs* args = (SortArgs*)arg;
    int* localCount = calloc(10, sizeof(int));
    
    for (int i = args->start; i < args->end; i++)
        localCount[(args->arr[i] / args->exp) % 10]++;
    
    pthread_barrier_wait(&barrier);
    
    int* output = malloc((args->end - args->start) * sizeof(int));
    int pos[10] = {0};
    
    for (int i = args->start; i < args->end; i++) {
        int digit = (args->arr[i] / args->exp) % 10;
        output[pos[digit]++] = args->arr[i];
    }
    
    for (int i = 0; i < (args->end - args->start); i++)
        args->arr[args->start + i] = output[i];
    
    free(output);
    free(localCount);
    return NULL;
}

void parallelRadixSort(int* arr, int size) {
    pthread_t threads[NUM_THREADS];
    SortArgs args[NUM_THREADS];
    int chunk = size / NUM_THREADS;
    
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    int max = getMax(arr, size);
    for (int exp = 1; max/exp > 0; exp *= 10) {
        for (int i = 0; i < NUM_THREADS; i++) {
            args[i].arr = arr;
            args[i].start = i * chunk;
            args[i].end = (i == NUM_THREADS - 1) ? size : (i + 1) * chunk;
            args[i].exp = exp;
            pthread_create(&threads[i], NULL, parallelCountSort, &args[i]);
        }
        
        for (int i = 0; i < NUM_THREADS; i++)
            pthread_join(threads[i], NULL);
        
        int* temp = malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) temp[i] = arr[i];
        countSort(temp, size, exp);
        for (int i = 0; i < size; i++) arr[i] = temp[i];
        free(temp);
    }
    
    pthread_barrier_destroy(&barrier);
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
    parallelRadixSort(arr, size);
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