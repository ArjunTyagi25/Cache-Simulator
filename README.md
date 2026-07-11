## Overview
This is a cache simulator.

## Project Structure
- `include/` - contains all the header files
- `src/` - contains all the source files
- `test/` - contains different tests used to test the simulator

## How to Build & Run
```
mkdir build
cd build/
cmake ..
make
./Cache-Simulator --memory_size <memory size in B> --page_size <page size in B> --cache_size <cache size in B> --line_size <line size in B> --assoc <associativity> --replacement_policy <specify the replacement policy for the cache> --write_policy <write_back/write_through> --write_allocate<true/false> --trace_file <path to trace file>"
```

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


