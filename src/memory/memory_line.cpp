#include <random>
#include <iostream>
#include "../../include/memory/memory_line.hpp"

using namespace std;

memory_line::memory_line(size_t line_size_, bool valid_, string init_)
{
    this->line_size = line_size_;

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<uint16_t> dist(0x00, 0xFF);
    for (size_t i = 0; i < this->line_size; i++)
    {
        if (init_ == "zeros")
            this->line_data.push_back(0x00);
        else if (init_ == "ones")
            this->line_data.push_back(0xFF);
        else if (init_ == "random")
            this->line_data.push_back(static_cast<uint8_t>(dist(generator)));
    }
    this->valid = valid_;
    this->dirty = false;
}

u_int8_t memory_line::get_byte(size_t offset_)
{
    return this->line_data[offset_];
}

vector<u_int8_t> memory_line::get_line_data()
{
    return this->line_data;
}

void memory_line::write_byte(u_int8_t write_data_, size_t offset_)
{
    this->line_data[offset_] = write_data_;
    this->valid = true;
    this->dirty = true;
}

void memory_line::write_line(vector<u_int8_t> write_data_)
{
    this->line_data = write_data_;
    this->valid = true;
    this->dirty = true;
}

bool memory_line::get_valid()
{
    return this->valid;
}

bool memory_line::get_dirty_bit()
{
    return this->dirty;
}

void memory_line::print_line_data()
{
    cout << this->valid << "\t" << this->dirty << "\t";
    for (size_t i = 0; i < this->line_size; i++)
    {
        cout << hex << static_cast<int>(this->line_data[i]) << " ";
    }
}