import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import os

def runSort(executable, arraySize, numProcs=None):
    startTime = time.time()
    
    if 'mpi' in executable:
        cmd = ['mpirun', '--oversubscribe', '-np', str(numProcs), f'./{executable}', str(arraySize)]
    else:
        cmd = [f'./{executable}', str(arraySize)]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        for line in result.stdout.split('\n'):
            if 'Time taken:' in line:
                return float(line.split(':')[1].strip().split()[0])
    except subprocess.CalledProcessError as e:
        print(f"Error running {executable}:")
        print(e.stderr)
        return None
    except Exception as e:
        print(f"Error running {executable}: {str(e)}")
        return None
    
    return time.time() - startTime

def runBenchmarks():
    arraySizes = [10000, 100000, 1000000]
    numRuns = 3
    
    results = {
        'sequential': [],
        'pthread': [],
        'openmp': [],
        'mpi': []
    }
    
    for arraySize in arraySizes:
        print(f"\nTesting array size: {arraySize}")
        
        sizeResults = {
            'sequential': [],
            'pthread': [],
            'openmp': [],
            'mpi': []
        }
        
        for _ in range(numRuns):
            timeSeq = runSort('sequential_quick', arraySize)
            if timeSeq: sizeResults['sequential'].append(timeSeq)
            
            timePthread = runSort('pthread_quick', arraySize)
            if timePthread: sizeResults['pthread'].append(timePthread)
            
            timeOpenmp = runSort('openmp_quick', arraySize)
            if timeOpenmp: sizeResults['openmp'].append(timeOpenmp)
            
            timeMpi = runSort('mpi_quick', arraySize, 4)
            if timeMpi: sizeResults['mpi'].append(timeMpi)
        
        for impl in results:
            if sizeResults[impl]:
                avgTime = np.mean(sizeResults[impl])
                results[impl].append(avgTime)
                print(f"{impl.capitalize()} average time: {avgTime:.4f} seconds")
            else:
                results[impl].append(None)
    
    return arraySizes, results

def plotResults(arraySizes, results):
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 1, 1)
    for impl, times in results.items():
        validPoints = [(size, time) for size, time in zip(arraySizes, times) if time is not None]
        if validPoints:
            xPoints, yPoints = zip(*validPoints)
            plt.plot(xPoints, yPoints, marker='o', label=impl.capitalize())
    
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Array Size')
    plt.ylabel('Time (seconds)')
    plt.title('Quick Sort Algorithm Performance Comparison')
    plt.grid(True)
    plt.legend()
    
    plt.subplot(2, 1, 2)
    seqTimes = results['sequential']
    for impl in ['pthread', 'openmp', 'mpi']:
        speedups = []
        validPoints = []
        for i, (parTime, seqTime) in enumerate(zip(results[impl], seqTimes)):
            if parTime is not None and seqTime is not None:
                speedups.append(seqTime / parTime)
                validPoints.append(arraySizes[i])
        if validPoints:
            plt.plot(validPoints, speedups, marker='o', label=impl.capitalize())
    
    plt.xscale('log')
    plt.xlabel('Array Size')
    plt.ylabel('Speedup (relative to sequential)')
    plt.title('Parallel Speedup Comparison')
    plt.grid(True)
    plt.legend()
    
    plt.tight_layout()
    
    os.makedirs('graphs', exist_ok=True)
    
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    plotFilename = f'graphs/quicksort_benchmark.png'
    plt.savefig(plotFilename)
    plt.close()

if __name__ == '__main__':
    print("Starting quick sort benchmarks...")
    arraySizes, results = runBenchmarks()
    plotResults(arraySizes, results)
    print("\nBenchmarking complete. Results have been saved to graphs/quicksort_benchmark.png")