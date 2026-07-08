#include<line.hpp>
#include<vector>

class cache
{
    public:
        cache(size_t cache_size_, size_t line_size_, size_t assoc_);
        u_int8_t find_byte(size_t address_);
        bool write_byte(size_t address_, u_int8_t write_data_);
        line* replace_line(line* new_line_, size_t address_);
        vector<line*> get_cache_lines();
        vector<vector<u_int8_t>> get_cache_data();


    private:
        size_t cache_size;
        size_t line_size;
        size_t assoc;
        size_t number_of_total_lines;
        size_t number_of_sets;
        size_t offset_bits;
        size_t offset_mask;
        size_t index_bits;
        size_t index_mask;
        vector<line*> cache_lines;
};