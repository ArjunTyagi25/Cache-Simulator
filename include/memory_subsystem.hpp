#pragma once

#include <string>
#include "./cache/cache.hpp"
#include "./memory/memory.hpp"
#include "./cache/cache_info.hpp"
#include "./memory/memory_info.hpp"

class memory_subsystem
{
    public:
        /*
        * @brief Constructor for memory_subsystem class
        * @param main_memory_size_ Size of the main memory in bytes
        * @param page_size_ Size of a page in bytes
        * @param L1_cache_size_ Size of the L1 cache in bytes
        * @param line_size_ Size of a line in bytes
        * @param assoc_ Associativity of the L1 cache
        * @param replacement_policy_ Replacement policy of L1 cache in case of conflict
        * @param write_policy_ Write policy of L1 cache
        * @param write_allocate_ Enable/disable write allocate in L1 cache
        * @param verbose_ Enable/disable verbose mode
        */
        memory_subsystem(size_t num_memory_levels_,
                         std::vector<MemoryInfo> memory_infos_,
                         size_t num_cache_levels_,
                         std::vector<CacheInfo> cache_infos_,
                         bool verbose_);
        
        /*
        * @brief Read an address
        * @param address_ Address of the location to be read
        * @return Returns the data read at address, either from cache or main memory
        */
        u_int8_t read(size_t address_);

        /*
        * @brief Write an address with given data
        * @param address_ Address of the location to be written
        * @param data_ Data to be written at the given address
        */
        void write(size_t address_, u_int8_t data_);

        /*
        * @brief Fill a cache line into the specified cache level
        * @param line_data_ Data bytes of the cache line to be installed.
        * @param address_ Address belonging to the cache line being installed.
        * @param cache_level_ Cache level where the line should be installed.
        */
        void fill_cache_line(std::vector<u_int8_t> line_data_, size_t address_, size_t cache_level_);

        /*
        * @brief Invalidate matching copies of a cache line in upper cache levels.
        * @param address_ Address belonging to the evicted cache line.
        * @param level_ Cache level that evicted the line; all levels above this are checked.
        */
        void invalidate_upper_level_copies(size_t address_, size_t level_);

        /*
        @brief Report all the cache statistics
        */
        void report_stats();

    private:
        size_t num_memory_levels;
        std::vector<std::string> memory_names;
        std::vector<size_t> memory_sizes;
        std::vector<size_t> page_sizes;
        std::vector<size_t> memory_line_sizes;

        size_t num_cache_levels;
        std::vector<std::string> cache_names;
        std::vector<size_t> cache_sizes;
        std::vector<size_t> cache_line_sizes;
        std::vector<size_t> assocs;
        std::vector<std::string> replacement_policies;
        std::vector<std::string> write_policies;
        std::vector<bool> write_allocates;
        bool verbose;
        
        std::vector<cache*> caches;
        std::vector<memory*> memories;

        std::vector<size_t> memory_line_offset_bits;
        std::vector<size_t> memory_line_offset_masks;

        std::vector<size_t> cache_line_offset_bits;
        std::vector<size_t> cache_line_offset_masks;
        
        std::vector<size_t> index_bits;
        std::vector<size_t> index_masks;

        std::vector<size_t> page_offset_bits;
        std::vector<size_t> page_offset_masks;
};

