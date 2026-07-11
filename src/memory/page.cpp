#include <iostream>
#include <cmath>
#include "../../include/memory/page.hpp"

using namespace std;

page::page(size_t page_size_, size_t line_size_, string init_)
{
    this->page_size = page_size_;
    this->line_size = line_size_;

    if (this->page_size % this->line_size != 0)
        cout << "Page size is not a multiple of line size" << endl;

    this->number_of_lines = this->page_size / this->line_size;

    this->page_offset_bits = log2(this->page_size);
    this->page_offset_mask = (1u << this->page_offset_bits) - 1;

    this->line_offset_bits = log2(this->line_size);
    this->line_offset_mask = (1u << this->line_offset_bits) - 1;

    this->line_index_bits = log2(this->number_of_lines);
    this->line_index_mask = (1u << this->line_index_bits) - 1;
    
    for (size_t i = 0; i < this->number_of_lines; i++)
    {
        memory_line* l = new memory_line(this->line_size, true, init_);
        this->page_lines.push_back(l);
    }
}

memory_line* page::get_line(size_t address_)
{
    size_t line_index = (address_ >> this->line_offset_bits) & this->line_index_mask;
    return this->page_lines[line_index];
}

void page::write_byte(u_int8_t write_data_, size_t address_)
{
    size_t line_index = (address_ >> this->line_offset_bits) & this->line_index_mask;
    size_t line_offset = address_ & line_offset_mask;

    this->page_lines[line_index]->write_byte(write_data_, line_offset); 
}

void page::write_line(vector<u_int8_t> line_data_, size_t address_)
{
    size_t line_index = (address_ >> this->line_offset_bits) & this->line_index_mask;
    this->page_lines[line_index]->write_line(line_data_);
}

void page::print_page_data()
{
    cout << "Line #\tValid\tDirty\tData" << endl;
    for (size_t i = 0; i < this->number_of_lines; i++)
    {
        cout << i << "\t";
        this->page_lines[i]->print_line_data();
        cout << endl;
    }
}

