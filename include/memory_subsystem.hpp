#pragma once

#include <string>
#include <optional>

#include "./cache/cache.hpp"
#include "./memory/memory.hpp"
#include "./cache/cache_info.hpp"
#include "./memory/memory_info.hpp"
#include "./memory_management_unit/address_translation_info.hpp"
#include "./memory_management_unit/memory_management_unit.hpp"

class memory_subsystem
{
    private:
        /// @brief Number of levels of memory
        size_t num_memory_levels;
        /// @brief Name of each memory level
        std::vector<std::string> memory_names;
        /// @brief Size of each memory level in bytes (B)
        std::vector<size_t> memory_sizes;
        /// @brief Size of page in each memory level in bytes (B)
        std::vector<size_t> page_sizes;
        /// @brief Size of memory line in each memory level in bytes (B)
        std::vector<size_t> memory_line_sizes;
        /// @brief Read latency of each memory level in cycles
        std::vector<size_t> memory_read_latencies;
        /// @brief Write latency of each memory level in cycles
        std::vector<size_t> memory_write_latencies;
        /// @brief Number of levels of cache
        size_t num_cache_levels;
        /// @brief Name of each cache level
        std::vector<std::string> cache_names;
        /// @brief Size of each cache level in bytes (B)
        std::vector<size_t> cache_sizes;
        /// @brief Size of cache line in each cache level in bytes (B)
        std::vector<size_t> cache_line_sizes;
        /// @brief Associativity of each cache level
        std::vector<size_t> assocs;
        /// @brief Replacement policy of each cache level
        std::vector<std::string> replacement_policies;
        /// @brief Write policy of each cache level
        std::vector<std::string> write_policies;
        /// @brief Write allocate of each cache level
        std::vector<bool> write_allocates;
        /// @brief Read latency of each cache level in cycles
        std::vector<size_t> cache_read_latencies;
        /// @brief Write latency of each cache level in cycles
        std::vector<size_t> cache_write_latencies;
        /// @brief `true` if verbose mode is enabled; `false` otherwise
        bool verbose;
        /// @brief `vector` holding all the cache levels
        std::vector<cache*> caches;
        /// @brief `vector` holding all the memory levels
        std::vector<memory*> memories;
        /// @brief The memory management unit
        memory_management_unit* MMU;
        /// @brief Number of line offset bits in each memory level
        std::vector<size_t> memory_line_offset_bits;
        /// @brief Mask to extract memory line offset for each memory level
        std::vector<size_t> memory_line_offset_masks;
        /// @brief Number of cache line offset bits in each cache level
        std::vector<size_t> cache_line_offset_bits;
        /// @brief Mask to extract cache line offset for each cache level
        std::vector<size_t> cache_line_offset_masks;
        /// @brief Number of cache index bits for each cache level
        std::vector<size_t> index_bits;
        /// @brief Mask to extract cache index for each cache level
        std::vector<size_t> index_masks;
        /// @brief Number of page offset bits in each memory level
        std::vector<size_t> page_offset_bits;
        /// @brief Mask to extract page offset for each memory level
        std::vector<size_t> page_offset_masks;
        /// @brief Number of reads at each cache level
        std::vector<size_t> cache_levels_read;
        /// @brief Number of writes at each cache level
        std::vector<size_t> cache_levels_write;
        /// @brief Number of reads at each memory level
        std::vector<size_t> memory_levels_read;
        /// @brief Number of writes at each memory level
        std::vector<size_t> memory_levels_write;
        /// @brief Number of bits in the virtual address
        size_t num_bits_virtual_address;
        /// @brief Number of bits in the physical address
        size_t num_bits_physical_address;
        /// @brief Number of levels in the page table
        size_t num_levels_page_table;
        /// @brief Size of a page table entry (PTE) in bytes (B)
        size_t PTE_size;
        /// @brief Size of the TLB in bytes (B)
        size_t TLB_size;
        /// @brief Allocation policy when requesting a free physical frame from memory
        std::string allocation_policy;
        
        /**
         * @brief Insert the requested cache line in all levels of cache hierarchy upon a read hit/miss
         * 
         * @param source_line_data_ Data of the line that contains `address_`
         * @param cache_hit_level_ Cache level at which hit happened. If missed across all cache levels, it will be equal to the number of levels in the cache hierarchy
         * @param address_ Address of the location that was read
         */
        void read_fill_path(std::vector<u_int8_t> source_line_data_, size_t cache_hit_level_, size_t address_);

        /**
         * @brief Insert a cache line into the specified cache level
         * 
         * @param line_data_ Data bytes of the cache line to be installed.
         * @param address_ Address belonging to the cache line being installed.
         * @param dirty_bit_ Dirty bit of the cache line to be installed
         * @param cache_level_ Cache level where the line should be installed.
         */
        void insert_line_at_cache_level(std::vector<u_int8_t> line_data_, size_t address_, bool dirty_bit_, size_t cache_level_);

        /**
         * @brief Insert/updates all levels of cache hierarchy upon a write hit/miss
        * 
         * @param source_line_data_ Data of the line that contains `address_`. This data should be before `write_data_` is written into the line
         * @param cache_hit_level_ Cache level at which hit happened. If missed across all cache level, it will be equal to the number of levels in the cache hierarchy
         * @param address_ Address of the location to which write should take place
         * @param write_data_ Data to write at the given address
         * @param hit_status_ true in case of write hit at any cache level, false in case of write miss acros all cache levels
         */
        bool write_fill_path(std::vector<u_int8_t> source_line_data_, size_t cache_hit_level_, size_t address_, u_int8_t write_data_, bool hit_status_);

        /**
         * @brief Invalidate matching copies of a cache line in upper cache levels.
         * 
         * @param address_ Address belonging to the evicted cache line.
         * @param level_ Cache level that evicted the line; all levels above this are checked.
         */
        void invalidate_upper_level_copies(size_t address_, size_t level_);

        /**
         * @brief Updates the total latency variable
         */
        void update_latency();

    public:
        /// @brief Current total of the total latency for a given trace
        size_t total_latency;
        
        /**
         * @brief Constructor for memory_subsystem class
         * 
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
                         AddressTranslationInfo address_translation_info,
                         bool verbose_);

        /**
         * @brief Destroy the memory subsystem object
         */
        ~memory_subsystem();
        
        /**
         * @brief Read an address
         * 
         * @param virtual_address_ Virtual address of the location to be read
         * @return Returns the data read at address, either from cache or main memory
         */
        u_int8_t read(size_t virtual_address_);

        /**
         * @brief Write an address with given data
         * 
         * @param virtual_address_ Virtual address of the location to be written
         * @param data_ Data to be written at the given address
         */
        void write(size_t virtual_address_, u_int8_t data_);

        /**
         * @brief Report all the cache statistics
         */
        void report_stats();
};

