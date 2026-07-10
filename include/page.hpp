#pragma once

#include<vector>
#include<string>
#include<line.hpp>

class page
{
    public:
        /*
        * @brief Constructor for page class
        * @param page_size_ Size of the page in bytes
        * @param line_size_ Size of the line in bytes
        * @param init_ Decide how to initialize the page. Options are `zeros`, `one`, or 'random'
        */
        page(size_t page_size_, size_t line_size, std::string init_);

        /*
        * @brief Get a line from the page
        * @param address_ Address of the byte in the line that needs to be read
        * @return Return a pointer of type `line*` based on the given address
        */
        line* get_line(size_t address_);

        /*
        * @brief Write a line in the page
        * @param l_ `line*` pointer to the line
        * @param address_ Address to any byte within the line from the line offset will be calculated
        */
        void write_line(line* l_, size_t address_);

        /*
        * @brief Prints the page's data line-by-line
        */
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
