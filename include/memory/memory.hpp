#pragma once

#include<vector>
#include<string>
#include "page.hpp"

class memory
{
    public:
        /*
        * @brief Constructor for the memory class
        * @param memory_size_ Size of the memory
        * @param page_size_ Size of the page within the memory
        * @param line_size_ Size of the line within a page
        * @param init_ Decide how to initialize the memory. Options are `zeros`, `one`, or 'random'
        */
        memory(size_t memory_size_, size_t page_size_, size_t line_size, std::string init_);

        /*
        * @brief Read a line from the memory
        * @param address_ Address of the byte within the line that needs to be read
        * @return Pointer to the line of type `line*`
        */
        memory_line* get_line(size_t address_);

        /*
        * @brief Write a byte to memory
        * @param write_data_ Data to be written
        * @param address_ Address of the location where data is to be written
        */
        void write_byte(u_int8_t write_data_, size_t address_);

        /*
        * @brief Update a line in the memory
        * @param write_data Data of the entire line that is to be written to the memory
        * @param address_ Address to any byte within the line from which the page number will be calculated
        */
        void write_line(std::vector<u_int8_t> write_data_, size_t address_);

        /*
        * @brief Prints all the memory's content
        */
        void print_memory_data();

    private:
        size_t memory_size;
        size_t page_size;
        size_t line_size;
        size_t number_of_pages;
        size_t page_offset_bits;
        std::vector<page*> pages;
};