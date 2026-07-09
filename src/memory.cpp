#include<iostream>
#include<memory.hpp>

using namespace std;

memory::memory(size_t memory_size_, size_t page_size_, size_t line_size_, string init_)
{
    this->memory_size = memory_size_;
    this->page_size = page_size_;
    this->line_size = line_size_;

    if (this->memory_size % this->page_size != 0)
        cout << "Memory size is not a multiple of page size" << endl;

    this->number_of_pages = this->memory_size / this->page_size;
    this->page_offset_bits = log2(this->page_size);
    
    for (size_t i = 0; i < this->number_of_pages; i++)
    {
        page* p = new page(this->page_size, this->line_size, init_);
        this->pages.push_back(p);
    }
}

line* memory::get_line(size_t address_)
{
    size_t page_number = address_ >> this->page_offset_bits;

    return this->pages[page_number]->get_line(address_);
}

void memory::write_line(line* l_, size_t address_)
{
    size_t page_number = address_ >> this->page_offset_bits;

    this->pages[page_number]->write_line(l_, address_);
}

void memory::print_memory_data()
{
    for (size_t i = 0; i < this->number_of_pages; i++)
    {
        cout << "Page #: " << i << endl;
        this->pages[i]->print_page_data();
        cout << endl;
    }
}