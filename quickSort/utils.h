#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateRandomArray(int* arr, int size) {
    for (int i = 0; i < size; i++) arr[i] = rand() % 10000;
}

inline void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

int medianOfThree(int* arr, int low, int high) {
    int mid = low + (high - low) / 2;
    
    if (arr[mid] < arr[low]) swap(&arr[mid], &arr[low]);
    if (arr[high] < arr[low]) swap(&arr[high], &arr[low]);
    if (arr[high] < arr[mid]) swap(&arr[high], &arr[mid]);
    
    swap(&arr[mid], &arr[high - 1]);
    return arr[high - 1];
}

int isSorted(int* arr, int size) {
    for (int i = 1; i < size; i++) if (arr[i] < arr[i-1]) return 0;
    return 1;
}

#endif