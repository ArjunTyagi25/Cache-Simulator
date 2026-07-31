#include <cstdio>
#include <vector>

#include "../../include/address_translation/page_table.hpp"

using namespace std;

page_table::page_table(size_t page_size_,
                       size_t page_table_entry_size_,
                       bool last_level_page_table_)
{
    this->num_entries = page_size_/page_table_entry_size_;

    for (size_t i = 0; i < this->num_entries; i++)
    {
        if (last_level_page_table_)
        {
            page_table_entry* PTE = new page_table_entry(0, false, false);
            this->page_table_.push_back(PTE);
        }
        else
        {
            this->page_table_.push_back(monostate{});
        }
    }
}

variant<monostate, page_table*, page_table_entry*> page_table::find_entry(size_t index_)
{
    // Entry in the page table is empty/NULL
    if (std::holds_alternative<std::monostate>(this->page_table_[index_]))
    {
        return std::monostate{};
    }
    // Entry in the page table is a pointer to lower-level page table
    else if (std::holds_alternative<page_table*>(this->page_table_[index_]))
    {
        return std::get<page_table*>(this->page_table_[index_]);
    }
    // Entry in the page table is a PTE pointing to a physical page number
    else if (std::holds_alternative<page_table_entry*>(this->page_table_[index_]))
    {
        return std::get<page_table_entry*>(this->page_table_[index_]);
    }
    else
        return std::monostate{};
}

void page_table::insert_entry(size_t index_,
                              variant<page_table_entry*, page_table*> entry_)
{
    if (std::holds_alternative<page_table_entry*>(entry_))
    {
        this->page_table_[index_] = std::get<page_table_entry*>(entry_);
    }
    else if (std::holds_alternative<page_table*>(entry_))
    {
        this->page_table_[index_] = std::get<page_table*>(entry_);
    }
}