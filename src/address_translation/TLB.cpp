#include <iostream>

#include "../../include/address_translation/TLB.hpp"

using namespace std;

TLB::TLB(size_t num_entries_)
{
    this->num_entries = num_entries_;
    this->gen = mt19937(0);
}

page_table_entry* TLB::find_entry(size_t VPN_)
{
    if (this->TLB_content.find(VPN_) != this->TLB_content.end())
        return this->TLB_content[VPN_];
    else
        return nullptr;
}

page_table_entry* TLB::insert_entry(size_t VPN_,
                                    page_table_entry* entry_)
{
    // Check if the VPN_ already existed in the TLB
    if (this->TLB_content.find(VPN_) != this->TLB_content.end())
    {
        this->TLB_content[VPN_] = entry_;
        return nullptr;
    }
    // Since the VPN_ does not exist in the TLB, we have to insert it. We first check if the current number of entries in the TLB is less than the size of TLB. If so, we just insert the new entry and return nullptr to indict nothing was evicted
    else if (this->TLB_content.size() < this->num_entries)
    {
        this->TLB_content[VPN_] = entry_;
        return nullptr;
    }
    else if (this->TLB_content.size() == this->num_entries)
    {
        uniform_int_distribution<size_t> dist(0, this->num_entries);
        size_t entry_to_evict = dist(this->gen);
        page_table_entry* evicted_entry = this->TLB_content[entry_to_evict];
        this->TLB_content[VPN_] = entry_;
        return evicted_entry;
    }
    else
    {
        cout << "Number of entries in the TLB exceed the maximum size. Please check your code implementation!" << endl;
        return nullptr;
    }
}