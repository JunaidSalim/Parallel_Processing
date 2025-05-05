#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
    
    if ((n & (n - 1)) != 0) {
        if (rank == 0) printf("Array size must be power of 2\n");
        MPI_Finalize();
        return 1;
    }
    
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

    bitonicSort(localArr, 0, elementsPerProc, 1);

    for (int step = 2; step <= size; step *= 2) {
        int partner = rank ^ (step - 1);
        int dir = (rank & step) != 0;
        
        int* recvBuf = malloc(elementsPerProc * sizeof(int));
        MPI_Sendrecv(localArr, elementsPerProc, MPI_INT, partner, 0,
                     recvBuf, elementsPerProc, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int* merged = malloc(2 * elementsPerProc * sizeof(int));
        int i = 0, j = 0, k = 0;
        
        while (i < elementsPerProc && j < elementsPerProc) {
            if (dir == (localArr[i] > recvBuf[j])) {
                merged[k++] = recvBuf[j++];
            } else {
                merged[k++] = localArr[i++];
            }
        }
        while (i < elementsPerProc) merged[k++] = localArr[i++];
        while (j < elementsPerProc) merged[k++] = recvBuf[j++];
        
        for (i = 0; i < elementsPerProc; i++)
            localArr[i] = merged[i + (dir ? elementsPerProc : 0)];
        
        free(recvBuf);
        free(merged);
    }

    int* sortedArr = NULL;
    if (rank == 0) sortedArr = malloc(n * sizeof(int));

    MPI_Gather(localArr, elementsPerProc, MPI_INT,
               sortedArr, elementsPerProc, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
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