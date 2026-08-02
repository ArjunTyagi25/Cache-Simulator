#include "../../include/memory_management_unit/page_table_entry.hpp"

using namespace std;

page_table_entry::page_table_entry(size_t PPN_,
                                   bool dirty_bit_,
                                   bool valid_bit_)
{
    this->PPN = PPN_;
    this->dirty_bit = dirty_bit_;
    this->valid_bit = valid_bit_;
}

void page_table_entry::update_entry(size_t PPN_,
                                    bool dirty_bit_,
                                    bool valid_bit_)
{
    this->PPN = PPN_;
    this->dirty_bit = dirty_bit_;
    this->valid_bit = valid_bit_;
}

size_t page_table_entry::get_PPN()
{
    return this->PPN;
}

bool page_table_entry::get_dirty_bit()
{
    return this->dirty_bit;
}

bool page_table_entry::get_valid_bit()
{
    return this->valid_bit;
}

void page_table_entry::set_dirty_bit(bool dirty_bit_)
{
    this->dirty_bit = dirty_bit_;
}

void page_table_entry::set_valid_bit(bool valid_bit_)
{
    this->valid_bit = valid_bit_;
}