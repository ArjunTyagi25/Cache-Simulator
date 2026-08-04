#include <iostream>
#include <string>
#include <bitset>
#include <optional>

#include "../../include/memory_management_unit/memory_management_unit.hpp"

using namespace std;

memory_management_unit::memory_management_unit(size_t num_bits_virtual_address_,
                                               size_t num_bits_physical_address_,
                                               size_t num_levels_page_table_,
                                               size_t PTE_size_,
                                               size_t TLB_size_,
                                               size_t page_size_,
                                               string allocation_policy_)
{
    this->num_bits_virtual_address = num_bits_virtual_address_;
    this->num_bits_physical_address = num_bits_physical_address_;
    this->num_levels_page_table = num_levels_page_table_;
    this->PTE_size = PTE_size_;
    this->TLB_size = TLB_size_;
    this->page_size = page_size_;
    this->allocation_policy = allocation_policy_;

    this->page_offset_bits = log2(this->page_size);
    this->page_offset_mask = (1u << this->page_offset_bits) - 1;

    this->num_PTE_per_page_table = this->page_size/this->PTE_size;
    this->total_num_of_page_tables = (pow(this->num_PTE_per_page_table, this->num_levels_page_table-1) - 1)/(this->num_PTE_per_page_table - 1);
    this->num_bits_in_per_level_index = (this->num_bits_virtual_address - this->page_offset_bits)/this->num_levels_page_table;

    if (this->num_levels_page_table == 1)
        this->root_table = new page_table(this->num_PTE_per_page_table, false);
    else
        this->root_table = new page_table(this->num_PTE_per_page_table, true);

    this->tlb = new TLB(this->TLB_size/this->PTE_size);
}

memory_management_unit::~memory_management_unit()
{
    delete this->root_table;
    delete this->tlb;
}

size_t memory_management_unit::address_translate(size_t VA_,
                                                 bool is_write_,
                                                 vector<memory*> memories_)
{
    size_t VPN = VA_ >> this->page_offset_bits;
    size_t page_offset = VA_ & this->page_offset_mask;
    size_t PFN = 0;

    page_table_entry* PTE = this->tlb->find_entry(VPN);

    if (PTE == nullptr)
    {
        // Miss in the TLB so need to perform page table walk, and then insert the entry in the TLB
        vector<size_t> page_table_indices;

        for (size_t level = this->num_levels_page_table; level > 0; level--)
        {
            size_t VPN_index = (VPN >> (this->num_bits_in_per_level_index * (level - 1))) && ((1u << this->num_bits_in_per_level_index) - 1);
            page_table_indices.push_back(VPN_index);
        }
        PTE = this->page_table_walk(page_table_indices);

        // Page fault occured. 
        if (PTE == nullptr)
        {
            // Request a physical frame and insert its entry into the page table
            PFN = this->request_physical_page(memories_);
            this->insert_entry_in_page_table(VPN, PFN, is_write_, page_table_indices);

            // Similarly, update the TLB with the new entry, and if an entry was evicted, write it back to the page table
            page_table_entry* evicted_PTE = this->tlb->update_entry(VPN, PFN, is_write_, true);

            if (evicted_PTE != nullptr)
            {

            }
        }
        else
        {
            PFN = PTE->get_PFN();
            PTE->set_dirty_bit(is_write_);
            this->tlb->update_entry(VPN, PFN, is_write_, true);
        }
    }
    else
    {
        // Hit in the TLB
        PFN = PTE->get_PFN();
        this->tlb->update_entry(VPN, PFN, is_write_, true);
    }

    size_t PA = (PFN << this->page_offset_bits) | page_offset;

    return PA;
}

page_table_entry* memory_management_unit::page_table_walk(vector<size_t> page_table_indices_)
{
    page_table* p = this->root_table;
    variant<monostate, page_table*, page_table_entry*> entry;

    for (size_t level = 0; level < this->num_levels_page_table; level++)
    {
        entry = p->find_entry(page_table_indices_[level]);

        if (holds_alternative<monostate>(entry))
        {
            return nullptr;
        }
        else if (holds_alternative<page_table*>(entry))
        {    
            p = get<page_table*>(entry);
        }
        else
        {
            return get<page_table_entry*>(entry);
        }
    }

    return nullptr;
}

size_t memory_management_unit::request_physical_page(vector<memory*> memories_)
{
    return memories_[0]->get_free_physical_frame(this->allocation_policy);
}

void memory_management_unit::insert_entry_in_page_table(size_t VPN_,
                                                        size_t PFN_,
                                                        bool dirty_bit_,
                                                        vector<size_t> page_table_indices_)
{
    page_table* miss_level_page_table = this->root_table;
    variant<monostate, page_table*, page_table_entry*> entry;
    size_t miss_level = 0;

    // Find the level in the page table at which miss occurred
    for (size_t level = 0; level < this->num_levels_page_table; level++)
    {
        entry = miss_level_page_table->find_entry(page_table_indices_[level]);

        if (holds_alternative<monostate>(entry))
        {
            miss_level = level;
            break;
        }
        else if (holds_alternative<page_table*>(entry))
        {    
            miss_level_page_table = get<page_table*>(entry);
        }
    }

    // Starting from the level at which miss occurred, insert a new page table or a page table entry depending upon the level at which you are currently.
    for (size_t level = miss_level; level < this->num_levels_page_table; level++)
    {
        if (level != this->num_levels_page_table - 1)
        {
            page_table* new_table = new page_table(this->num_PTE_per_page_table, false);
            miss_level_page_table->insert_entry(page_table_indices_[level], new_table);
            miss_level_page_table = new_table;
        }
        else
        {
            page_table_entry* new_entry = new page_table_entry(VPN_, PFN_, dirty_bit_, true);
            miss_level_page_table->insert_entry(page_table_indices_[level], new_entry);
        }
    }
}

void memory_management_unit::print_TLB_data()
{
    this->tlb->print_TLB_data();
}