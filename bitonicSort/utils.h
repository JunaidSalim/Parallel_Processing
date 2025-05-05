#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateRandomArray(int* arr, int size) {
    for (int i = 0; i < size; i++) arr[i] = rand() % 10000;
}

void compareAndSwap(int* arr, int i, int j, int dir) {
    if (dir == (arr[i] > arr[j])) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void bitonicMerge(int* arr, int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        for (int i = low; i < low + k; i++)
            compareAndSwap(arr, i, i + k, dir);
        bitonicMerge(arr, low, k, dir);
        bitonicMerge(arr, low + k, k, dir);
    }
}

int isSorted(int* arr, int size) {
    for (int i = 1; i < size; i++) if (arr[i] < arr[i-1]) return 0;
    return 1;
}

#endif