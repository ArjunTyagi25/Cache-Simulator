## Overview
This is a cache simulator.

## Project Structure
- `include/` - contains all the header files
- `src/` - contains all the source files
- `test/` - contains different tests used to test the simulator

## Compiling The Simulator
```
mkdir build
cd build/
cmake ..
make
```

## Running The Simulator
After compiling the simulator, there are two ways to run it:
1. Directly from the command line
2. Using the provided Bash script

### 1. Running Directly from the Command Line

To run the simulator directly, use:
```
./Cache-Simulator \ 
    --memory_size <memory size in B> \ 
    --page_size <page size in B> \ 
    --cache_size <cache size in B> \ 
    --line_size <line size in B> \ 
    --assoc <associativity> \
    --replacement_policy <specify the replacement policy for the cache> \
    --write_policy <write_back/write_through> \
    --write_allocate<true/false> \
    --trace_file <path to trace file> \
    --verbose <true/false>
```

Currently supported replacement policies are:
```
first_line
random
LFU
MFU
```

Currently supported write policies are:
```
write_back
write_through
```

### 2. Running with the Bash Script
Alternatively, you can run the simulator using the provided Bash script:
```
./run_simulator.sh <path to the YAML config file>
```

The YAML config file specifies the simulator parameters, including memory size, cache size, line size, associativity, replacement policy, write policy, write-allocation behavior, verbosity, and trace file path. A sample configuration file is provided in the root directory as `sample_config.yaml`.

## Features to Implement
- [x] Detailed statistics
- [x] Verbose mode
- [x] Different write policies
- [ ] LRU, pseudo-LRU and FIFO replacement policy
- [ ] Add cache and memory latency models
- [ ] Multi-level caches
- [ ] Update trace to support multi-byte read/write operations
- [ ] Distinguishing between virtual and physical address
- [ ] Translating virtual address to physical address
- [ ] Lockup-free cache implementation