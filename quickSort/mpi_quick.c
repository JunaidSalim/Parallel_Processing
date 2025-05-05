#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "utils.h"

void quickSort(int* arr, int low, int high) {
    if (low < high) {
        int pivot = medianOfThree(arr, low, high);
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main(int argc, char* argv[]) {
    int rank, size, n;
    double startTime, endTime;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 2) {
        if (rank == 0) printf("Usage: mpirun -np <num_processes> %s <array_size>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    n = atoi(argv[1]);
    
    if (n % size != 0) {
        if (rank == 0) printf("Array size must be divisible by number of processes\n");
        MPI_Finalize();
        return 1;
    }

    int* globalArr = NULL;
    if (rank == 0) {
        globalArr = malloc(n * sizeof(int));
        srand(time(NULL));
        generateRandomArray(globalArr, n);
        
        printf("First few elements before sorting: ");
        for(int i = 0; i < (n < 10 ? n : 10); i++) printf("%d ", globalArr[i]);
        printf("\n");
    }

    int elementsPerProc = n / size;
    int* localArr = malloc(elementsPerProc * sizeof(int));

    startTime = MPI_Wtime();
    
    MPI_Scatter(globalArr, elementsPerProc, MPI_INT,
                localArr, elementsPerProc, MPI_INT,
                0, MPI_COMM_WORLD);

    quickSort(localArr, 0, elementsPerProc - 1);

    int* tempArr = NULL;
    if (rank == 0) tempArr = malloc(n * sizeof(int));

    MPI_Gather(localArr, elementsPerProc, MPI_INT,
               tempArr, elementsPerProc, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        int* sortedArr = malloc(n * sizeof(int));
        int* aux = malloc(n * sizeof(int));
        int step = 1;
        
        for (int i = 0; i < n; i++) sortedArr[i] = tempArr[i];
        
        while (step < size) {
            for (int i = 0; i < size; i += 2 * step) {
                int left = i * elementsPerProc;
                int mid = (i + step) * elementsPerProc - 1;
                int right = ((i + 2 * step) * elementsPerProc - 1) < n ? 
                           ((i + 2 * step) * elementsPerProc - 1) : n - 1;
                
                if (mid < n) {
                    int k = left;
                    int i1 = left;
                    int i2 = mid + 1;
                    
                    while (i1 <= mid && i2 <= right) {
                        if (sortedArr[i1] <= sortedArr[i2])
                            aux[k++] = sortedArr[i1++];
                        else
                            aux[k++] = sortedArr[i2++];
                    }
                    while (i1 <= mid) aux[k++] = sortedArr[i1++];
                    while (i2 <= right) aux[k++] = sortedArr[i2++];
                    
                    for (k = left; k <= right; k++)
                        sortedArr[k] = aux[k];
                }
            }
            step *= 2;
        }
        
        endTime = MPI_Wtime();
        
        printf("First few elements after sorting: ");
        for(int i = 0; i < (n < 10 ? n : 10); i++) printf("%d ", sortedArr[i]);
        printf("\n");
        
        printf("Array is sorted: %s\n", isSorted(sortedArr, n) ? "Yes" : "No");
        printf("Time taken: %f seconds\n", endTime - startTime);
        
        free(sortedArr);
        free(aux);
        free(tempArr);
        free(globalArr);
    }

    free(localArr);
    MPI_Finalize();
    return 0;
}