# Technical Documentation

## Architecture

### High-Level Flow

```
┌─────────────────────────────────────────────┐
│         User Input Validation               │
│  (seed, iterations, verbose output)         │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│      Start High-Resolution Timer            │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│     OpenMP Parallel Region                  │
│  ┌──────────────────────────────────────┐   │
│  │ Each Thread:                          │   │
│  │ • Create local RNG (MT19937)         │   │
│  │ • Roll die N/T times (N=iterations) │   │
│  │ • Update local_dieNumber array      │   │
│  │ • Optionally print to console       │   │
│  └────────────────┬─────────────────────┘   │
│                   │                          │
│  ┌────────────────▼─────────────────────┐   │
│  │ Critical Section:                     │   │
│  │ Merge local_dieNumber into global    │   │
│  └────────────────┬─────────────────────┘   │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│    Calculate Statistics                     │
│  • Count occurrences per die face           │
│  • Calculate percentage for each           │
│  • Compute mean average error              │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│   Append Results to data.txt                │
│   Display Results to User                   │
└─────────────────────────────────────────────┘
```

### Parallel Strategy

The application uses **data parallelism** with OpenMP:

```c++
#pragma omp parallel
{
    // Each thread gets unique seed offset
    std::mt19937 mt_local(seed + omp_get_thread_num());
    std::uniform_int_distribution<int> die(1, 6);
    long long int local_dieNumber[6]{0};  // Private to each thread
    
    #pragma omp for
    for (long long int i = 0; i < iterations; ++i) {
        int value = die(mt_local);
        local_dieNumber[value - 1]++;
    }
    
    #pragma omp critical  // Only one thread at a time
    {
        for (long long int i = 0; i < 6; ++i)
            dieNumber[i] += local_dieNumber[i];
    }
}
```

**Key Design Decisions:**

1. **Local Arrays**: Each thread maintains its own `local_dieNumber` to minimize synchronization overhead
2. **Critical Section**: Only aggregation uses critical section, not individual increments
3. **Seed Uniqueness**: Each thread gets `seed + thread_number` for different sequences
4. **Work Distribution**: Loop iterations automatically distributed across threads by `#pragma omp for`

### Fairness Calculation

The fairness metric measures how close the observed distribution is to the perfect theoretical distribution:

```c++
double mean = total / 6.0;  // Expected: iterations / 6

error = (
    (modulus(dieNumber[0] - mean) +
     modulus(dieNumber[1] - mean) +
     modulus(dieNumber[2] - mean) +
     modulus(dieNumber[3] - mean) +
     modulus(dieNumber[4] - mean) +
     modulus(dieNumber[5] - mean)) / 6.0
) * 100.0 / mean;

accuracy = 100 - error;  // Percentage accuracy
```

**Formula Breakdown:**

1. **Mean**: Total outcomes ÷ 6 faces = expected count per face
2. **Deviation**: Sum of absolute differences between observed and expected
3. **Average Deviation**: Sum ÷ 6
4. **Percentage Error**: (Average Deviation ÷ Mean) × 100
5. **Accuracy**: 100% - Error%

**Why this metric?**
- Normalized to iterations count
- Easy to compare across different run sizes
- Ranges from 0% (perfect) to 100%+ (highly skewed)

### Random Number Generation

Uses the Mersenne Twister algorithm via C++ standard library:

```c++
std::mt19937 mt_local(seed + omp_get_thread_num());
std::uniform_int_distribution<int> die(1, 6);
```

**Properties:**
- Period: 2^19937-1 (extremely long)
- Suitable for statistical simulations
- Hardware efficient
- **Thread-safe**: Each thread has its own generator instance

**Why offset the seed?**
- Ensures different pseudo-random sequences per thread
- Avoids correlation between threads
- Maintains reproducibility (same seed → same sequence per thread)

## File Operations

### Writing Results

Located in `files.cpp::writeFile()`:

```c++
std::ofstream outFile(fileName, std::ios::app);  // Append mode
outFile << "iterations: " << NoOfPermutations 
         << " random number chosen: " << random 
         << " percentage mean average error: " << error 
         << " time taken: " << elapsed << '\n';
```

**Format**: Tab-separated values on a single line for easy parsing

### Reading Results

Located in `files.cpp::readFile()`:

```c++
std::ifstream inFile(fileName);
while (std::getline(inFile, myText)) {
    std::cout << lineNumber++ << ": " << myText << '\n';
}
```

Displays all historical runs with line numbers for easy reference.

### Data Deletion

Implemented as password-protected feature:

```c++
if (iterations == password && seed == password)  // password = 10212
{
    deleteData("data.txt");  // Truncates file
}
```

## Input Validation

Comprehensive error handling for user inputs:

```c++
while (true) {
    std::cout << "\nEnter any random positive integer: ";
    std::cin >> seed;
    
    if (clearFailedExtraction()) {
        continue;  // Invalid input, retry
    }
    
    // Proceed with valid input
    break;
}
```

**Handles:**
- Non-integer inputs
- EOF conditions
- Stream failures
- Automatic recovery and retry

## Performance Characteristics

### Time Complexity

- **Core Loop**: O(N) where N = iterations
- **Thread Distribution**: Parallelized across P cores
- **Aggregation**: O(1) critical section per thread
- **Statistics Calculation**: O(1) constant time
- **Overall**: O(N/P) with P parallel threads

### Space Complexity

- **Global**: O(1) - only 6-element array + scalars
- **Per-Thread Local**: O(1) - only 6-element array + RNG state
- **File I/O**: O(1) append operations
- **Overall**: O(1) constant space

### Scaling Characteristics

Speedup typically follows this pattern:

```
Cores → Speedup ≈ 0.8-0.95 × Cores
```

The sub-linear scaling is due to:
- OpenMP overhead
- Memory bandwidth limitations
- Cache effects
- Thread synchronization costs

### Typical Performance (Modern CPU)

| Iterations | Time (seconds) | Error % | Accuracy % |
|-----------|--------|---------|-----------|
| 10^6 | 0.001 | 1-3% | 97-99% |
| 10^7 | 0.01 | 0.3-1% | 99-99.7% |
| 10^8 | 0.1 | 0.1-0.3% | 99.7-99.9% |
| 10^9 | 1-2 | 0.01-0.1% | 99.9-99.99% |
| 10^10 | 10-20 | 0.003-0.01% | 99.99-99.997% |

## Building and Compilation

### Project Structure

```
dice-stats/
├── Project1.sln              # Visual Studio solution
├── Project1/
│   ├── main.cpp             # Main program
│   ├── files.cpp            # File I/O functions
│   ├── Project1.vcxproj     # Visual C++ project
│   ├── Project1.vcxproj.filters
│   ├── Project1.vcxproj.user
│   └── data.txt             # Results file
├── x64/                      # Build output
└── README.md
```

### Compiler Requirements

**Visual Studio (MSVC):**
- C++17 or later
- OpenMP support enabled
- /O2 optimization recommended for release builds

**GCC/Clang:**
```bash
g++ -std=c++17 -fopenmp -O2 main.cpp files.cpp -o dice-stats
```

### Optimization Flags

**Release Build (Recommended):**
```
/O2           # Optimize for speed
/Ob2          # Inline function expansion
/Oi           # Enable intrinsic functions
/Ot           # Favor speed
```

## Observed Behavior Analysis

### Convergence Pattern

From the collected data in `data.txt`:

1. **Large Iteration Runs**:
   - 99.9 billion iterations: 0.00056725% error
   - 9.9 billion iterations: 0.0009288% error
   - 8.8 billion iterations: 0.00125297% error

2. **Error Decay**:
   ```
   Error ∝ 1/√N
   ```
   - 10x more iterations ≈ 3.16x lower error
   - 100x more iterations ≈ 10x lower error

3. **Execution Time**:
   - Scales approximately linearly with iterations
   - Some variance due to system load and thermal throttling

## Known Limitations

1. **32-bit Integer Overflow**: For extremely large iteration counts (>2^63), long long may overflow
2. **File I/O**: No buffering strategy; each run appends individually
3. **Memory**: While space-efficient, extremely high thread counts may cause issues
4. **Seed Offset**: Simple seed offset doesn't guarantee maximum statistical independence
5. **Output**: Verbose output (-O0) is extremely slow for large iterations

## Future Optimizations

1. **SIMD Vectorization**: Use AVX-512 for faster random number generation
2. **Memory Mapping**: Use mmap for large result files
3. **Batch Processing**: Group dice rolls for better cache locality
4. **Adaptive Threading**: Dynamic thread count based on system load
5. **GPU Acceleration**: CUDA kernels for 1000x+ speedup on consumer GPUs

## Thread Safety Analysis

### Thread-Safe Components

✅ **RNG per-thread instance**: Each thread has private generator  
✅ **Local arrays**: Private to each thread, no sharing  
✅ **File I/O**: Only in main thread after parallel section  
✅ **Aggregation**: Protected by #pragma omp critical

### Potential Race Conditions

⚠️ **Global dieNumber array**: Protected during aggregation only  
⚠️ **cout output**: May interleave output, but safe (no corruption)

### Analysis

**Verdict**: The code is thread-safe for its current implementation.

## Benchmarking Methodology

To use as CPU benchmark:

1. Run with identical parameters on different systems
2. Compare execution time
3. Factor: Time = Iterations / (CPU Speed × Parallelism)
4. Results in data.txt show relative performance

**Benchmark Running Protocol:**

```bash
# Run 3 times, take average
./dice-stats.exe  # 1 billion iterations

# Compare total time across different CPUs
```

---

For questions or contributions, refer to the main README.md file.
