#pragma once

#include <optional>
#include <vector>
#include <random>
#include <deque>

#include "cache_line.hpp"

class cache
{
    private:
        /// @brief Size of the cache in bytes (B)
        size_t cache_size;
        /// @brief Size of the cache line in bytes (B)
        size_t line_size;
        /// @brief Associativity of the cache
        size_t assoc;
        /// @brief Replacement policy of the cache
        std::string replacement_policy;
        /// @brief Total number of cache lines in the cache
        size_t number_of_total_lines;
        /// @brief Total number of sets in the cache
        size_t number_of_sets;
        /// @brief Number of offset bits in the address
        size_t offset_bits;
        /// @brief Mask to extract the offset from an address
        size_t offset_mask;
        /// @brief Number of index bits in the address
        size_t index_bits;
        /// @brief Mask to extract the index from an address
        size_t index_mask;
        /// @brief Generator for random number
        std::mt19937 gen;
        /// @brief Vector holding all the cache lines that make up the cache
        std::vector<cache_line*> cache_lines;
        /// @brief Number of accesses to each cache line in the cache
        std::vector<int> access_counts;
        /// @brief Vector of queues to keep track of access order for each set in the cache
        std::vector<std::deque<size_t>> access_order;
        /// @brief Vector of queues to keep track of pseudo-access order for each set in the cache
        std::vector<std::vector<bool>> pseudo_access_order;
        /// @brief Vector of queses to keep track of insert order for each set in the cache
        std::vector<std::deque<size_t>> insert_order;

        /**
         * @brief Implement different eviction (a.k.a., replacement) policies
         * 
         * @param index_ Index of the set from which a line is to be evicted
         * @return Line number to be evicted from the cache, based on the eviction policy
         */
        size_t eviction_policy(size_t index_);

        /**
         * @brief Updates the history of all lines in a set 
         * 
         * @param index_ Index of the set whose history is supposed to be updated
         * @param line_number_ Line number that was accessed
         */
        void update_access_history(size_t index_, 
                                   size_t line_number_);

        /**
         * @brief Updates the insert history i.e., the order in which lines are inserted in a set
         * 
         * @param index_ Index of the set whose insert history is supposed to be updated
         * @param line_number_ Line number that was inserted
         */
        void update_insert_history(size_t index_, 
                                   size_t line_number_);

    public:
        /// @brief Total number of accesses to the cache
        size_t total_accesses;
        /// @brief Total number of read accesses to the cache
        size_t read_accesses;
        /// @brief Total number of write accesses to the cache
        size_t write_accesses;
        /// @brief Total number of read hits in the cache
        size_t read_hits;
        /// @brief Total number of read misses in the cache
        size_t read_misses;
        /// @brief Total number of write hits in the cache
        size_t write_hits;
        /// @brief Total number of write misses in the cache
        size_t write_misses;
        /// @brief Total number of hits in the cache
        size_t total_hits;
        /// @brief Total number of misses in the cache
        size_t total_misses;
        /// @brief Total number of evictions from the cache
        size_t total_evictions;
        /// @brief Total number of dirty evictions from the cache
        size_t dirty_evictions;
        /// @brief Total number of non-dirty evictions from the cache
        size_t non_dirty_eviction;
        /// @brief Hit rate of the cache
        double hit_rate;
        /// @brief Miss rate of the cache
        double miss_rate;

        /**
         * @brief Constructor for cache class
         * 
         * @param cache_size_ Size of the cache
         * @param line_size_ Size of a line in the cache
         * @param assoc_ Associativity of the cache
         * @param replacement_policy_ Replacement policy of the cache in case of cache conflict miss
         */
        cache(size_t cache_size_, 
              size_t line_size_, 
              size_t assoc_, 
              std::string replacement_policy_);

        /**
         * @brief Destroy the cache object
         */
        ~cache();
        
        /**
         * @brief Read a byte of data in the cache
         * 
         * @param address_ Address of the byte to be read
         * @return `uint8_t` on read hit, `nullopt` on read miss 
         */
        std::optional<uint8_t> read_byte(size_t address_);

        /**
         * @brief Check if the cache holds an address. 
         * 
         * @details This is different from read_byte as find_byte() would not update the cache statistics. Used for write operations.
         * @param address_ Address of the byte that needs to be checked
         * @return `true` if found, `false` if not found
         */
        bool find_byte(size_t address_);
 
        /**
         * @brief Write a cache line (assuming it is already present in the cache)
         * 
         * @param line_data_ Data of the line that is to be written
         * @param address_ Address of the line; used to extract index and tag
         * @param dirty_bit_ Dirty bit of the line that is to be written
         */
        void write(std::vector<u_int8_t> line_data_, 
                   size_t address_, 
                   bool dirty_bit_);

        /**
         * @brief Update a cache line that was evicted from a higher level cache. It does not update the `access_order` variable used by replacement policies like LRU, MRU, etc.
         * 
         * @param line_data_ Data of the line that is to be updated
         * @param address_ Address of the line; used to extract index and tag
         * @param dirty_bit_ Dirty bit of the line that is to be updated
         */
        void update_evicted_line(std::vector<u_int8_t> line_data_, 
                                 size_t address_, 
                                 bool dirty_bit_);

        /**
         * @brief Place a line of data in the cache
         * 
         * @param write_data_ Data of the line that needs to be placed in the cache
         * @param address_ Address of the line that needs to be placed in the cache
         * @param dirty_bit_ Dirty bit of the line that needs to be placed in the cache
         * @return Evicted line's data along with its address
         */
        std::optional<std::tuple<std::vector<u_int8_t>, size_t, bool>> insert_line(std::vector<u_int8_t> write_data_, 
                                                                                   size_t address_, 
                                                                                   bool dirty_bit_);

        /**
         * @brief Get a cache line
         * 
         * @param address_ Address of any byte that is in the line we want to fetch
         * @return Pointer to cache_line containing the address_
         */
        std::optional<cache_line*> get_cache_line(size_t address_);
        
        /**
         * @brief Get all the cache lines
         * 
         * @return `vector<line*>` containing all the lines of the cache
         */
        std::vector<cache_line*> get_cache_lines();

        /**
         * @brief Get the data stored in the cache
         * 
         * @return vector<vector<u_int8_t>> containing all the data of the cache
         */
        std::vector<std::vector<u_int8_t>> get_cache_data();

        /**
         * @brief Update the cache statistics in case of a write hit
         * 
         * @param address_ Address of the byte that was accesses. Used to extract tag and index which are then used to update the access count of the specific cache line
         */
        void update_write_hit_stats(size_t address_);

        /**
         * @brief Updat the cache statistics in case of a write miss
         */
        void update_write_miss_stats();

        /**
         @brief Print the cache data, including the metadata like tags, dirty bit, valid bit, etc
         */
        void print_cache_data();
};