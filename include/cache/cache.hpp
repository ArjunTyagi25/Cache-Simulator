#pragma once

#include <optional>
#include <vector>
#include <random>
#include "cache_line.hpp"

class cache
{
    public:
        // Cache stats
        size_t total_accesses;
        size_t read_accesses;
        size_t write_accesses;

        size_t read_hits;
        size_t read_misses;
        size_t write_hits;
        size_t write_misses;

        size_t total_hits;
        size_t total_misses;
        double hit_rate;
        double miss_rate;

        /*
        * @brief Constructor for cache class
        * @param cache_size_ Size of the cache
        * @param line_size_ Size of a line in the cache
        * @param assoc_ Associativity of the cache
        * @param replacement_policy_ Replacement policy of the cache in case of cache conflict miss
        */
        cache(size_t cache_size_, size_t line_size_, size_t assoc_, std::string replacement_policy_);
        
        /*
        * @brief Find a byte of data in the cache
        * @param address_ Address of the byte to be read
        * @return `uint8_t` on read hit, `nullopt` on read miss 
        */
        std::optional<uint8_t> find_byte(size_t address_);

        /*
        * @brief Write a byte of data in the cache
        * @param address_ Address of the byte to be written
        * @param write_data Data to be written at the given address
        * @return `true` on write hit, `false` on write miss  
        */
        bool write_byte(size_t address_, u_int8_t write_data_);

        /*
        * @brief Place a line of data in the cache
        * @param write_data_ Data of the line that needs to be placed in the cache
        * @param address_ Address of the line that needs to be placed in the cache
        * @return Evicted line's data along with its address
        */
        std::optional<std::pair<std::vector<u_int8_t>, size_t>> place_line(std::vector<u_int8_t> write_data_, size_t address_);

        /*
        * @brief Implement different eviction (a.k.a., replacement) policies
        * @param index Index of the set from which a line is to be evicted
        * @return Line number to be evicted from the cache, based on the eviction policy
        */
        size_t eviction_policy(size_t index);

        /*
        * @brief Get all the cache lines
        * @return `vector<line*>` containing all the lines of the cache
        */
        std::vector<cache_line*> get_cache_lines();

        /*
        * @brief Get the data stored in the cache
        * @return vector<vector<u_int8_t>> containing all the data of the cache
        */
        std::vector<std::vector<u_int8_t>> get_cache_data();

        /*
        @brief Print the cache data, including the metadata like tags, dirty bit, valid bit, etc
        */
        void print_cache_data();


    private:
        size_t cache_size;
        size_t line_size;
        size_t assoc;
        std::string replacement_policy;
        size_t number_of_total_lines;
        size_t number_of_sets;
        size_t offset_bits;
        size_t offset_mask;
        size_t index_bits;
        size_t index_mask;
        std::mt19937 gen;
        std::vector<cache_line*> cache_lines;
        std::vector<int> access_counts;
};