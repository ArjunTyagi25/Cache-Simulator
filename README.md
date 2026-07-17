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
    --num_memory_levels <number of memory levels> \
    --memory_level <name, memory size, page size, line size> \
    --num_cache_levels <number of cache levels> \
    --cache_level <name, cache size, line size, associativity, replacement policy, write policy, write allocate> \
    --trace_file <path to trace file> \
    --verbose <true/false>
```

For example, assuming one level of memory along with two levels of cache hierarchy,
```
./Cache-Simulator \
    --num_memory_levels 1 \
    --memory_level MAIN,128,32,4 \
    --num_cache_levels 2 \
    --cache_level L1,8,4,1,first_line,write_through,false \
    --cache_level L2,32,4,2,random,write_back,true \
    --trace_file ../test/sample_trace.txt \
    --verbose true
```

Currently supported replacement policies are:
```
first_line
random
LFU
MFU
LRU
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
- [x] Multi-level caches
- [ ] Update trace to support multi-byte read/write operations
- [ ] Distinguishing between virtual and physical address
- [ ] Translating virtual address to physical address
- [ ] Lockup-free cache implementation
