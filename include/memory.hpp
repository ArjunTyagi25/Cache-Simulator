#pragma once

#include<vector>
#include<page.hpp>

class memory
{
    public:
        memory(size_t memory_size_, size_t page_size_, size_t line_size, std::string init_);
        line* get_line(size_t address_);
        void write_line(line* l_, size_t address_);
        void print_memory_data();

    private:
        size_t memory_size;
        size_t page_size;
        size_t line_size;
        size_t number_of_pages;
        size_t page_offset_bits;
        std::vector<page*> pages;
};