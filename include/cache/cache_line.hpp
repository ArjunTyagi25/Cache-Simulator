#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>

class cache_line
{
    public:
        /*
        * @brief Constructor for cache_line class
        * @param line_size_ Size of the line
        * @param valid_ Valid bit for the line
        * @param init_ Decide how to initialize the line. Options are `zeros`, `one`, or 'random'
        */
        cache_line(size_t line_size_, bool valid_, std::string init_);

        /*
        * @brief Get a byte from the cache line
        * @param offset_ Offset in the line
        * @return Returns a byte of data of type `u_int8_t` from the line
        */
        u_int8_t get_byte(size_t offset_);

        /*
        * @brief Get a entire line's data
        * @return Returns the line's data as `vector<u_int8_t>`
        */
        std::vector<u_int8_t> get_line_data();
        
        /*
        * @brief Write a byte of data in the line
        * @param write_data_ Data to the written
        * @param tag_ Associated tag
        * @param offset_ Offset within the line where the data is to be written
        */
        void write_byte(u_int8_t write_data_, size_t tag_, size_t offset_);

        /*
        * @brief Update the entire line's data
        * @param write_data_ New line's data to be written
        * @param tag_ Associated tag
        * @param dirty_bit_ Dirty bit of the line. false if no changes have been made to the line's content; true if the line has been updated
        */
        void write_line(std::vector<u_int8_t> write_data_, size_t tag_, bool dirty_bit_);

        /*
        * @brief Returns the line's tag
        * @return `size_t` tag
        */
        size_t get_tag();

        /*
        * @brief Set the line's tag
        * @param tag_ Tag that we want to set for the line
        */
        void set_tag(size_t tag_);

        /*
        * @brief Set the line's valid bit
        * @param valid_ value of valid bit that you wanna set (true/false)
        */
        void set_valid(bool valid_);

        /*
        * @brief Get the valid bit of the line
        * @return Returns the valid bit of type `bool`
        */
        bool get_valid();

        /*
        * @brief Get the dirty bit of the line
        * @return Returns the dirty bit of type `bool`
        */
        bool get_dirty_bit();

        /*
        * @brief Print the line's data
        */
        void print_line_data();

    private:
        std::vector<u_int8_t> line_data;
        size_t tag;
        bool valid; 
        bool dirty;
        size_t line_size;
};

