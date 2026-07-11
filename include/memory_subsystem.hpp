#pragma once

#include <string>
#include "./cache/cache.hpp"
#include "./memory/memory.hpp"

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
        memory_subsystem(size_t main_memory_size_, 
                         size_t page_size_,
                         size_t L1_cache_size_,
                         size_t line_size_,
                         size_t assoc_,
                         std::string replacement_policy_,
                         std::string write_policy_,
                         bool write_allocate_,
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
        @brief Report all the cache statistics
        */
        void report_stats();

    private:
        size_t main_memory_size;
        size_t page_size;
        size_t L1_cache_size;
        size_t line_size;
        size_t assoc;
        std::string replacement_policy;
        std::string write_policy;
        bool write_allocate;
        bool verbose;
        cache* L1;
        memory* main_memory;

        size_t line_offset_bits;
        size_t line_offset_mask;
        
        size_t index_bits;
        size_t index_mask;

        size_t page_offset_bits;
        size_t page_offset_mask;
};

