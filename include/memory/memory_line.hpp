#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>

class memory_line
{
    private:
        /// @brief Size of the memory line in bytes (B)
        size_t line_size;
        /// @brief Valid bit associated with the memory line
        bool valid; 
        /// @brief Dirty bit associated with the memory line
        bool dirty;
        /// @brief Vector holding all the memory line's data
        std::vector<u_int8_t> line_data;

    public:
        /**
         * @brief Constructor for memory line class
         * 
         * @param line_size_ Size of the line
         * @param valid_ Valid bit for the line
         * @param init_ Decide how to initialize the line. Options are `zeros`, `one`, or 'random'
         */
        memory_line(size_t line_size_, bool valid_, std::string init_);

        /**
         * @brief Destroy the memory line object
         */
        ~memory_line() = default;

        /**
         * @brief Get a byte from the line
         * 
         * @param offset_ Offset in the line
         * @return Returns a byte of data of type `u_int8_t` from the line
         */
        u_int8_t get_byte(size_t offset_);

        /**
         * @brief Get a entire line's data
         * 
         * @return Returns the line's data as `vector<u_int8_t>`
         */
        std::vector<u_int8_t> get_line_data();
        
        /**
         * @brief Write a byte of data in the line
         * 
         * @param write_data_ Data to the written
         * @param offset_ Offset within the line where the data is to be written
         */
        void write_byte(u_int8_t write_data_, size_t offset_);

        /**
         * @brief Update the entire line's data
         * 
         * @param write_data_ New line's data to be written
         */
        void write_line(std::vector<u_int8_t> write_data_);

        /**
         * @brief Get the valid bit of the line
         * 
         * @return Returns the valid bit of type `bool`
         */
        bool get_valid();

        /**
         * @brief Get the dirty bit of the line
         * 
         * @return Returns the dirty bit of type `bool`
         */
        bool get_dirty_bit();

        /**
         * @brief Print the line's data
         */
        void print_line_data();
};