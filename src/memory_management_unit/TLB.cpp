#include <iostream>

#include "../../include/memory_management_unit/TLB.hpp"

using namespace std;

TLB::TLB(size_t num_entries_)
{
    this->num_entries = num_entries_;

    for (size_t i = 0; i < this->num_entries; i++)
    {
        page_table_entry* PTE = new page_table_entry(0, 0, false, false);
        this->TLB_content.push_back(PTE);
    }
    this->gen = mt19937(0);
}

TLB::~TLB()
{
    for (size_t i = 0; i < this->num_entries; i++)
    {
        delete this->TLB_content[i];
    }
    this->TLB_content.clear();
}

page_table_entry* TLB::find_entry(size_t VPN_)
{
    for (size_t i = 0; i < this->num_entries; i++)
    {
        if ((this->TLB_content[i]->get_VPN() == VPN_) & this->TLB_content[i]->get_valid_bit())
            return this->TLB_content[i];
    }
    return nullptr;
}

page_table_entry* TLB::update_entry(size_t VPN_,
                                    size_t PFN_,
                                    bool dirty_bit_,
                                    bool valid_bit_)
{
    // If the VPN_ already existed in the TLB
    for (size_t i = 0; i < this->num_entries; i++)
    {
        if ((this->TLB_content[i]->get_VPN() == VPN_) & this->TLB_content[i]->get_valid_bit())
        {
            this->TLB_content[i]->update_entry(VPN_, PFN_, dirty_bit_, valid_bit_);
            return nullptr;
        }
    }

    // VPN_ does not exist in the TLB. First, we check if there is an entry with valid bit as false
    for (size_t i = 0; i < this->num_entries; i++)
    {
        if (!this->TLB_content[i]->get_valid_bit())
        {
            this->TLB_content[i]->update_entry(VPN_, PFN_, dirty_bit_, valid_bit_);
            return nullptr;
        }
    }

    // VPN_ does not exist in the TLB and there is no empty space, so need to evict an entry from the TLB
    uniform_int_distribution<size_t> dist(0, this->num_entries);
    size_t entry_to_evict = dist(this->gen);
    
    size_t evicted_entry_VPN = this->TLB_content[entry_to_evict]->get_VPN();
    size_t evicted_entry_PFN = this->TLB_content[entry_to_evict]->get_PFN();
    bool evicted_entry_dirty_bit = this->TLB_content[entry_to_evict]->get_dirty_bit();
    bool evicted_entry_valid_bit = this->TLB_content[entry_to_evict]->get_valid_bit();

    this->TLB_content[entry_to_evict]->update_entry(VPN_, PFN_, dirty_bit_, valid_bit_);
    page_table_entry* evicted_entry = new page_table_entry(evicted_entry_VPN, evicted_entry_PFN, evicted_entry_dirty_bit, evicted_entry_valid_bit);
    return evicted_entry;
}

void TLB::print_TLB_data()
{
    cout << "Entry #\tValid\tDirty\tVPN\tPFN" << endl;
    for (size_t i = 0; i < this->num_entries; i++)
    {
        cout << i << "\t" << this->TLB_content[i]->get_valid_bit() << "\t" << this->TLB_content[i]->get_dirty_bit() << "\t" << hex << this->TLB_content[i]->get_VPN() << "\t" << this->TLB_content[i]->get_PFN() << endl; 
    }
}