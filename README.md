# Parallel Sorting Algorithms Implementation

This project implements four different sorting algorithms (Bitonic Sort, Merge Sort, Quick Sort, and Radix Sort) with various parallel programming paradigms including Sequential, Pthreads, OpenMP, and MPI.

## Project Structure

```
Project/
├── bitonicSort/    # Bitonic Sort implementations
├── mergeSort/      # Merge Sort implementations
├── quickSort/      # Quick Sort implementations
├── radixSort/      # Radix Sort implementations
└── Report.pdf      # Detailed analysis and results
```

Each sorting algorithm folder contains:
- Sequential implementation (sequential_*.c)
- Pthread implementation (pthread_*.c)
- OpenMP implementation (openmp_*.c)
- MPI implementation (mpi_*.c)
- Utility functions (utils.h)
- Benchmark script (benchmark.py)

## Prerequisites

- GCC compiler
- OpenMP support
- MPICH or OpenMPI
- Python 3.x with required packages:
  - numpy
  - matplotlib

## Installation

Install Python dependencies:
```bash
pip install numpy matplotlib
```

## Compilation Instructions

### Bitonic Sort
```bash
cd bitonicSort
gcc sequential_bitonic.c -o sequential_bitonic
gcc pthread_bitonic.c -o pthread_bitonic -pthread
gcc openmp_bitonic.c -o openmp_bitonic -fopenmp
mpicc mpi_bitonic.c -o mpi_bitonic
```

### Merge Sort
```bash
cd mergeSort
gcc sequential_merge.c -o sequential_merge
gcc pthread_merge.c -o pthread_merge -pthread
gcc openmp_merge.c -o openmp_merge -fopenmp
mpicc mpi_merge.c -o mpi_merge
```

### Quick Sort
```bash
cd quickSort
gcc sequential_quick.c -o sequential_quick
gcc pthread_quick.c -o pthread_quick -pthread
gcc openmp_quick.c -o openmp_quick -fopenmp
mpicc mpi_quick.c -o mpi_quick
```

### Radix Sort
```bash
cd radixSort
gcc sequential_radix.c -o sequential_radix
gcc pthread_radix.c -o pthread_radix -pthread
gcc openmp_radix.c -o openmp_radix -fopenmp
mpicc mpi_radix.c -o mpi_radix
```

## Running Individual Programs

Each program can be run with the following syntax:

### Sequential, Pthread, and OpenMP versions:
```bash
./<executable_name> <array_size>
```
Example:
```bash
./sequential_quick 1000000
```

### MPI versions:
```bash
mpirun -np <num_processes> ./<executable_name> <array_size>
```
Example:
```bash
mpirun -np 4 ./mpi_quick 1000000
```

## Running Benchmarks

Each sorting algorithm folder contains a benchmark.py script that compares the performance of different implementations. To run benchmarks:

```bash
cd <algorithm_folder>
python benchmark.py
```

For example:
```bash
cd quickSort
python benchmark.py
```

The benchmark will:
1. Run each implementation with different array sizes
2. Perform multiple runs for statistical accuracy
3. Generate performance comparison graphs
4. Save results in the 'graphs' subfolder

## Benchmark Results

Benchmark results are saved as PNG files in the respective 'graphs' folders:
- bitonicSort/graphs/bitonic_benchmark.png
- mergeSort/graphs/merge_benchmark.png
- quickSort/graphs/quicksort_benchmark.png
- radixSort/graphs/radix_benchmark.png

The graphs show:
- Execution time comparison for different implementations
- Speedup relative to sequential implementation

## Notes

- Bitonic Sort requires input sizes to be powers of 2
- MPI implementations require the array size to be divisible by the number of processes
- The benchmark script automatically uses 4 processes for MPI implementations