import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import os

def runSort(executable, size, numProcs=None):
    startTime = time.time()
    
    if 'mpi' in executable:
        cmd = ['mpirun', '--oversubscribe', f'-np', str(numProcs), f'./{executable}', str(size)]
    else:
        cmd = [f'./{executable}', str(size)]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error running {executable}:")
            print(result.stderr)
            return None
        
        for line in result.stdout.split('\n'):
            if 'Time taken:' in line:
                return float(line.split(':')[1].strip().split()[0])
    except Exception as e:
        print(f"Error running {executable}: {str(e)}")
        return None
    
    return time.time() - startTime

def runBenchmarks():
    sizes = [1024, 4096, 16384, 65536]
    numRuns = 3
    
    results = {
        'sequential': [],
        'pthread': [],
        'openmp': [],
        'mpi': []
    }
    
    for size in sizes:
        print(f"\nTesting array size: {size}")
        
        sizeResults = {
            'sequential': [],
            'pthread': [],
            'openmp': [],
            'mpi': []
        }
        
        for _ in range(numRuns):
            timeSeq = runSort('sequential_bitonic', size)
            if timeSeq:
                sizeResults['sequential'].append(timeSeq)
            
            timePthread = runSort('pthread_bitonic', size)
            if timePthread:
                sizeResults['pthread'].append(timePthread)
            
            timeOpenmp = runSort('openmp_bitonic', size)
            if timeOpenmp:
                sizeResults['openmp'].append(timeOpenmp)
            
            timeMpi = runSort('mpi_bitonic', size, 4)
            if timeMpi:
                sizeResults['mpi'].append(timeMpi)
        
        for impl in results:
            if sizeResults[impl]:
                avgTime = np.mean(sizeResults[impl])
                results[impl].append(avgTime)
                print(f"{impl.capitalize()} average time: {avgTime:.4f} seconds")
            else:
                results[impl].append(None)
    
    return sizes, results

def plotResults(sizes, results):
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 1, 1)
    for impl, times in results.items():
        validPoints = [(size, time) for size, time in zip(sizes, times) if time is not None]
        if validPoints:
            x, y = zip(*validPoints)
            plt.plot(x, y, marker='o', label=impl.capitalize())
    
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Array Size')
    plt.ylabel('Time (seconds)')
    plt.title('Bitonic Sort Algorithm Performance Comparison')
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
                validPoints.append(sizes[i])
        if validPoints:
            plt.plot(validPoints, speedups, marker='o', label=impl.capitalize())
    
    plt.xscale('log')
    plt.xlabel('Array Size')
    plt.ylabel('Speedup (relative to sequential)')
    plt.title('Parallel Speedup Comparison')
    plt.grid(True)
    plt.legend()
    
    plt.tight_layout()
    
    if not os.path.exists('graphs'):
        os.makedirs('graphs')
    
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    plt.savefig(f'graphs/bitonic_benchmark.png')
    plt.close()

if __name__ == '__main__':
    print("Starting bitonic sort benchmarks...")
    sizes, results = runBenchmarks()
    plotResults(sizes, results)
    print("\nBenchmarking complete. Results have been saved to a PNG file.")