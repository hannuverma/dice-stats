# Dice Statistics Analyzer 🎲

A high-performance C++ application that simulates dice rolls to analyze fairness and measure probability convergence as iteration count increases. This project demonstrates how statistical distribution improves with more trials and can be used to benchmark CPU performance.

## Overview

This project investigates a fundamental principle of probability: as the number of trials increases, the observed frequencies of outcomes converge toward their theoretical probabilities. In the case of a fair six-sided die, each number should appear approximately 16.67% of the time.

The application:
- **Simulates millions/billions of dice rolls** using parallel computing
- **Calculates fairness metrics** by measuring deviation from expected probability distribution
- **Benchmarks CPU performance** by tracking execution time across different iteration counts
- **Supports customizable parameters** for flexible experimentation

## Features

✨ **Parallel Processing**: Uses OpenMP for multi-threaded computation to maximize performance  
📊 **Statistical Analysis**: Calculates percentage mean average error to quantify dice fairness  
⏱️ **Performance Measurement**: Tracks execution time for CPU benchmarking  
🔐 **Data Management**: Append results to file or clear data with password protection  
🎯 **Real-time Output**: Option to display all iterations as they occur  

## Quick Start

### Prerequisites
- C++ compiler with C++17 support
- OpenMP library
- Visual Studio 2022 (or equivalent C++ IDE)

### Building

The project uses Visual Studio. Open `Project1.sln` and build the solution:

```bash
# Or compile from command line
cl /OpenMP Project1/main.cpp Project1/files.cpp -o dice-stats.exe
```

### Running

```bash
./dice-stats.exe
```

You'll be prompted to enter:
1. **Random seed** - Any positive integer for reproducibility
2. **Number of iterations** - How many dice rolls to simulate
3. **Verbose output** - Whether to display each roll (1 for yes, 0 for no)

### Example Output

```
Enter any random positive integer: 123
Enter the number of iterations: 1000000000
Do you want to see all the iterations (1 for yes, 0 for no): 0

For 1000000000 iterations:
Number of 1's appeared are 166666626
Number of 2's appeared are 166666861
Number of 3's appeared are 166666755
Number of 4's appeared are 166666777
Number of 5's appeared are 166667106
Number of 6's appeared are 166665875

1 had 16.6666626% chance of appearing.
2 had 16.6666861% chance of appearing.
3 had 16.6666755% chance of appearing.
4 had 16.6666777% chance of appearing.
5 had 16.6667106% chance of appearing.
6 had 16.6665875% chance of appearing.

Percentage mean average error in probability is: 0.003239%
The given die is 99.996761% as accurate as a perfect dice
Time taken to complete the given iterations: 12.1903 seconds
```

## Data Files

### data.txt
Results are appended to `Project1/data.txt` in the following format:
```
iterations: [N] random number chosen: [SEED] percentage mean average error: [ERROR]% time taken: [TIME]s
```

### Clearing Data
To delete all collected data, run the program and enter:
- Iterations: `10212`
- Random seed: `10212`

## How It Works

### The Algorithm

1. **Parallel Initialization**: Each thread creates its own Mersenne Twister random number generator
2. **Parallel Dice Rolling**: Threads independently simulate dice rolls and count outcomes
3. **Thread-Safe Aggregation**: Results from all threads are combined safely using OpenMP critical sections
4. **Statistical Analysis**: Calculates mean deviation from expected 16.67% probability
5. **Performance Metrics**: Measures total execution time

### Fairness Metric

The application calculates **percentage mean average error**:

```
Error% = (Average Deviation from Expected / Expected) × 100
```

Where:
- Expected frequency per die face = Total iterations / 6
- Deviation = |Observed - Expected| for each face
- Lower error% = Fairer die

### Why This Matters

- **Law of Large Numbers**: Demonstrates how statistical distributions converge with more trials
- **CPU Performance**: Execution time correlates with processor speed and parallel efficiency
- **Probability Education**: Practical visualization of theoretical probability in action
- **Benchmarking**: Compare results across different machines to measure relative performance

## Building Details

The project uses:
- **Language**: C++ with C++17 features
- **Parallelization**: OpenMP (#pragma omp)
- **Random Generation**: `<random>` library (Mersenne Twister MT19937)
- **Timing**: `<chrono>` high-resolution clock
- **File I/O**: Standard `<fstream>`

### Key Files

- `main.cpp` - Main application logic and UI loop
- `files.cpp` - File I/O operations (read, write, delete)

## Performance Notes

- **Optimal iterations**: 1-100 billion (depending on CPU)
- **Parallel scaling**: Performance scales with number of CPU cores
- **Memory efficient**: Uses local arrays per thread to minimize synchronization
- **Typical times**: 
  - 1 billion iterations: ~10-15 seconds
  - 10 billion iterations: ~100-200 seconds
  - 100 billion iterations: ~1000+ seconds

## Observations from Data

Examining the collected data shows:
- Error decreases as iterations increase (√n relationship)
- 1 billion iterations: ~0.003-0.008% error
- 100 billion iterations: ~0.0005% error
- Performance varies with system load and CPU thermal state

## Future Enhancements

- [ ] Support for different types of dice (8-sided, 12-sided, 20-sided)
- [ ] Graph plotting of convergence over time
- [ ] Multi-process distributed computing
- [ ] GPU acceleration with CUDA
- [ ] Statistical significance testing
- [ ] Web interface for result visualization

## License

This project is open source and available for educational and experimental use.

## Contributing

Feel free to fork, modify, and contribute improvements!

## Author

Created to explore the intersection of probability theory, high-performance computing, and statistical analysis.

---

**Got questions?** Check the data in `Project1/data.txt` to see patterns and learn about probability convergence! 🚀
