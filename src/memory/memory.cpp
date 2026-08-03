#include <iostream>
#include <cmath>
#include <random>

#include "../../include/memory/memory.hpp"

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

    this->gen = mt19937(0);
}

memory::~memory() 
{
    for (size_t i = 0; i < this->number_of_pages; i++)
    {
        delete this->pages[i];
    }
    this->pages.clear();
}

memory_line* memory::get_line(size_t address_)
{
    size_t page_number = address_ >> this->page_offset_bits;

    return this->pages[page_number]->get_line(address_);
}

void memory::write_byte(u_int8_t write_data_, size_t address_)
{
    size_t page_number = address_ >> this->page_offset_bits;
    
    this->pages[page_number]->write_byte(write_data_, address_);
}

void memory::write_line(vector<u_int8_t> write_data_, size_t address_)
{
    size_t page_number = address_ >> this->page_offset_bits;
    this->pages[page_number]->write_line(write_data_, address_);
}

size_t memory::get_free_physical_frame(string allocation_policy_)
{
    if (allocation_policy_ == "first_free_page")
    {
        for (size_t i = 0; i < this->number_of_pages; i++)
        {
            if (this->pages[i]->get_free_bit())
            {   
                this->pages[i]->set_free_bit(false);
                return i;
            }
        }

        throw std::runtime_error("Out of memory: no free pages available.");
    }
    else if (allocation_policy_ == "random")
    {
        vector<size_t> free_pages;

        for (size_t i = 0; i < this->number_of_pages; i++)
        {
            if (this->pages[i]->get_free_bit())
            {   
                free_pages.push_back(i);
            }
        }

        if (free_pages.empty()) 
        {
            throw std::runtime_error("Out of memory: no free pages available.");
        }

        uniform_int_distribution<size_t> dis(0, free_pages.size() - 1);
        size_t free_PFN = free_pages[dis(gen)];
        this->pages[free_PFN]->set_free_bit(false);
        return free_PFN;
    }
    else
    {
        throw invalid_argument(
            "Invalid memory allocation policy."
        );
    }
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
