#pragma once

#include<cstdint>
#include<vector>
#include<string>
#include<optional>

class line
{
    public:
        /*
        * @brief Constructor for line class
        * @param line_size_ Size of the line
        * @param valid_ Valid bit for the line
        * @param init_ Decide how to initialize the line. Options are `zeros`, `one`, or 'random'
        */
        line(size_t line_size_, bool valid_, std::string init_);

        /*
        * @brief Get a byte from the line
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
        */
        void write_line(std::vector<u_int8_t> write_data_, size_t tag_);

        /*
        * @brief Returns the line's tag
        * @return `size_t` if the line is present in the cache, `nullopt` if the line is present in the memory
        */
        std::optional<size_t> get_tag();

        /*
        * @brief Set the line's tag
        * @param tag_ `size_t` if the line is being sent to the cache, `nullopt` if the line is being sent to the memory
        */
        void set_tag(std::optional<size_t> tag_);

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
        * @param print_tag_ `true` if line is in cache so print the tags, `false` if the line is in memory
        */
        void print_line_data(bool print_tag_);

    private:
        std::vector<u_int8_t> line_data;
        std::optional<size_t> tag;
        bool valid; 
        bool dirty;
        size_t line_size;
};

