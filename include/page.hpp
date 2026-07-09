#pragma once

#include<vector>
#include<line.hpp>

class page
{
    public:
        page(size_t page_size_, size_t line_size, std::string init_);
        line* get_line(size_t address_);
        void write_line(line* l_, size_t address_);
        void print_page_data();

    private:
        std::vector<line*> page_lines;
        size_t page_size;
        size_t line_size;
        size_t number_of_lines;
        size_t page_offset_bits;
        size_t page_offset_mask;
        size_t page_number_mask;
};