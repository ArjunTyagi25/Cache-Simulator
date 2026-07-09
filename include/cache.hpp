#pragma once

#include<optional>
#include<vector>
#include<random>
#include<line.hpp>

class cache
{
    public:
        cache(size_t cache_size_, size_t line_size_, size_t assoc_, std::string replacement_policy_);
        std::optional<uint8_t> find_byte(size_t address_);
        bool write_byte(size_t address_, u_int8_t write_data_);
        std::pair<line*, size_t> replace_line(line* new_line_, size_t address_);
        size_t eviction_policy(size_t index);
        std::vector<line*> get_cache_lines();
        std::vector<std::vector<u_int8_t>> get_cache_data();
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
        std::vector<line*> cache_lines;
};