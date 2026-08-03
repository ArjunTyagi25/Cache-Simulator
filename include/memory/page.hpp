#pragma once

#include <vector>
#include <string>

#include "memory_line.hpp"

class page
{
    private:
        /// @brief Size of the page in bytes (B)
        size_t page_size;
        /// @brief Size of a memory line in bytes (B)
        size_t line_size;
        /// @brief Free bit associated with the page
        bool free;
        /// @brief Dirty bit associated with the page
        bool dirty;
        /// @brief Vector holding all the memory lines comprising the page
        std::vector<memory_line*> page_lines;
        /// @brief Nimber of memory lines in a page
        size_t number_of_lines;
        /// @brief Number of bits in the address for line offset
        size_t line_offset_bits;
        /// @brief Mask to extract the line offset from an address
        size_t line_offset_mask;
        /// @brief Number of bits in the address for line index
        size_t line_index_bits;
        /// @brief Mask to extract the line index from an address
        size_t line_index_mask;
    public:
        /**
         * @brief Constructor for page class
         * 
         * @param page_size_ Size of the page in bytes
         * @param line_size_ Size of the line in bytes
         * @param init_ Decide how to initialize the page. Options are `zeros`, `one`, or 'random'
         */
        page(size_t page_size_, size_t line_size, std::string init_);

        /**
         * @brief Destroy the page object
         */
        ~page();

        /**
         * @brief Get a line from the page
         * 
         * @param address_ Address of the byte in the line that needs to be read
         * @return Return a pointer of type `line*` based on the given address
         */
        memory_line* get_line(size_t address_);

        /**
         * @brief Write a byte to a line in the page
         * 
         * @param write_data_ `u_int8_t` Data to be written
         * @param address_ Address of the location where data is to be written
         */
        void write_byte(u_int8_t write_data_, size_t address_);

        /**
         * @brief Write a line in the page
         * 
         * @param line_data_ Data of the entire line that is to be written
         * @param address_ Address to any byte within the line from the line offset will be calculated
         */
        void write_line(std::vector<u_int8_t> line_data_, size_t address_);

        /**
         * @brief Prints the page's data line-by-line
         */
        void print_page_data();

        /**
         * @brief Set the free bit to a desired value
         * 
         * @param free_ Desired value that you want of the free bit
         */
        void set_free_bit(bool free_);

        /**
         * @brief Get the free bit 
         * 
         * @return `free` bit of the page
         */
        bool get_free_bit();

        /**
         * @brief Update the dirty bit of the page by checking the dirty bit of each line stored in the page
         */
        void update_dirty_bit();
};
