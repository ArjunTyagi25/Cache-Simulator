#include<page.hpp>
#include<iostream>
#include<cmath>

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
    
    for (size_t i = 0; i < this->number_of_lines; i++)
    {
        line* l = new line(this->line_size, true, init_);
        this->page_lines.push_back(l);
    }

}

line* page::get_line(size_t address_)
{
    size_t line_offset_bit = log2(this->line_size);
    size_t page_offset = address_ & this->page_offset_mask;
    size_t line_index = page_offset >> line_offset_bit;
    return this->page_lines[line_index];
}

void page::write_line(line* l_, size_t address_)
{
    size_t line_offset_bit = log2(this->line_size);
    size_t line_index = address_ >> line_offset_bit;

    this->page_lines[line_index] = l_;
    this->page_lines[line_index]->set_tag(nullopt);
}

void page::print_page_data()
{
    cout << "Line #\tValid\tDirty\tData" << endl;
    for (size_t i = 0; i < this->number_of_lines; i++)
    {
        cout << i << "\t";
        this->page_lines[i]->print_line_data(false);
        cout << endl;
    }
}

