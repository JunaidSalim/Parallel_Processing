#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "utils.h"

void radixSort(int* arr, int size) {
    int max = getMax(arr, size);
    for (int exp = 1; max/exp > 0; exp *= 10)
        countSort(arr, size, exp);
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

    radixSort(localArr, elementsPerProc);

    int* sortedArr = NULL;
    if (rank == 0) sortedArr = malloc(n * sizeof(int));

    MPI_Gather(localArr, elementsPerProc, MPI_INT,
               sortedArr, elementsPerProc, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        radixSort(sortedArr, n);
        endTime = MPI_Wtime();

        printf("First few elements after sorting: ");
        for(int i = 0; i < (n < 10 ? n : 10); i++) printf("%d ", sortedArr[i]);
        printf("\n");
        
        printf("Array is sorted: %s\n", isSorted(sortedArr, n) ? "Yes" : "No");
        printf("Time taken: %f seconds\n", endTime - startTime);
        
        free(sortedArr);
        free(globalArr);
    }

    free(localArr);
    MPI_Finalize();
    return 0;
}