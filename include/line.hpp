#pragma once

#include<cstdint>
#include<vector>

class line
{
    public:
        line(size_t line_size_, bool valid_, std::string init_);
        u_int8_t get_byte(size_t offset_);
        std::vector<u_int8_t> get_line_data();
        void write_byte(u_int8_t write_data_, size_t tag_, size_t offset_);
        void write_line(std::vector<u_int8_t> write_data_, size_t tag_);
        size_t get_tag();
        bool get_valid();
        bool get_dirty_bit();
        void print_line_data();

    private:
        std::vector<u_int8_t> line_data;
        size_t tag;
        bool valid; 
        bool dirty;
        size_t line_size;
};

