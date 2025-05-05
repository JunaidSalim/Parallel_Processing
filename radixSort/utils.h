#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateRandomArray(int* arr, int size) {
    for (int i = 0; i < size; i++) arr[i] = rand() % 10000;
}

inline int getMax(int* arr, int size) {
    int max = arr[0];
    for (int i = 1; i < size; i++) 
        if (arr[i] > max) max = arr[i];
    return max;
}

int isSorted(int* arr, int size) {
    for (int i = 1; i < size; i++) if (arr[i] < arr[i-1]) return 0;
    return 1;
}

void countSort(int* arr, int size, int exp) {
    int* output = (int*)malloc(size * sizeof(int));
    int count[10] = {0};

    for (int i = 0; i < size; i++) count[(arr[i] / exp) % 10]++;
    for (int i = 1; i < 10; i++) count[i] += count[i - 1];

    for (int i = size - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    for (int i = 0; i < size; i++) arr[i] = output[i];
    free(output);
}

#endif