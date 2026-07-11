#include <string>
#include <iostream>
#include <optional>

#include "../include/memory_subsystem.hpp"

using namespace std;

memory_subsystem::memory_subsystem(size_t main_memory_size_, 
                                   size_t page_size_,
                                   size_t L1_cache_size_,
                                   size_t line_size_,
                                   size_t assoc_,
                                   string replacement_policy_,
                                   string write_policy_,
                                   bool write_allocate_,
                                   bool verbose_)
{
    this->main_memory_size = main_memory_size_;
    this->page_size = page_size_;
    this->L1_cache_size = L1_cache_size_;
    this->line_size = line_size_;
    this->assoc = assoc_;
    this->replacement_policy = replacement_policy_;
    this->write_policy = write_policy_;
    this->write_allocate = write_allocate_;
    this->verbose = verbose_;

    this->L1 = new cache(this->L1_cache_size, this->line_size, this->assoc, this->replacement_policy);
    this->main_memory = new memory(this->main_memory_size, this->page_size, this->line_size, "random");

    cout << "--------------------INITIAL CONTENT OF MAIN MEMORY--------------------" << endl;
    this->main_memory->print_memory_data();
    cout << "--------------------INITIAL STATA OF L1 CACHE---------------------" << endl;
    this->L1->print_cache_data();
    cout << "---------------------------------------------------------------" << endl;

    this->line_offset_bits = log2(this->line_size);
    this->line_offset_mask = (1u << this->line_offset_bits) - 1;

    this->index_bits = log2(this->L1_cache_size/(this->line_size * this->assoc));
    this->index_mask = (1u << this->index_bits) - 1;

    this->page_offset_bits = log2(this->page_size);
    this->page_offset_mask = (1u << this->page_offset_bits) - 1;

}

u_int8_t memory_subsystem::read(size_t address_)
{
    string status;
    bool evicted = false;
    optional<u_int8_t> read_byte = this->L1->find_byte(address_);
    if (read_byte.has_value())
    {
        status = "READ HIT";

        if (verbose)
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "\tTrace: READ " << address_ << endl;
            cout << "\tTag: " << hex << (address_ >> (this->index_bits + this->line_offset_bits)) << endl;
            cout << "\tIndex: " << hex << ((address_ >> this->line_offset_bits) & this->index_mask) << endl;
            cout << "\tLine Offset: " << hex << (address_ & this->line_offset_mask) << endl;
            cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits) << endl;
            cout << "\tPage Offset: " << hex << (address_ & this->page_offset_mask) << endl;
            cout << "\tStatus: " << status << endl; 
            cout << "\tEvicted: " << evicted << endl;
            cout << "\tRead Value: " << static_cast<size_t>(*read_byte) << endl; 
            cout << "----------MAIN MEMORY'S CONTENTS----------" << endl;
            this->main_memory->print_memory_data();
            cout << "----------L1 CACHE'S CONTENTS----------" << endl;
            this->L1->print_cache_data();
            cout << "-----------------------------------------------------------" << endl;
        }

        return read_byte.value();
    }
    else
    {
        status = "READ MISS";
        memory_line* mem_line = this->main_memory->get_line(address_);
        vector<u_int8_t> mem_line_data = mem_line->get_line_data();

        size_t offset_bits = log2(this->line_size);
        size_t offset_mask = (1u << offset_bits) - 1;
        size_t offset = address_ & offset_mask;
        read_byte = mem_line_data[offset];

        optional<pair<vector<u_int8_t>, size_t>> evicted_line = this->L1->place_line(mem_line_data, address_);
        if (evicted_line.has_value())
        {
            vector<u_int8_t> evicted_line_data = evicted_line.value().first;
            this->main_memory->write_line(evicted_line_data, evicted_line.value().second);
            evicted = true;
        }

        if (verbose)
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "\tTrace: READ " << address_ << endl;
            cout << "\tTag: " << hex << (address_ >> (this->index_bits + this->line_offset_bits)) << endl;
            cout << "\tIndex: " << hex << ((address_ >> this->line_offset_bits) & this->index_mask) << endl;
            cout << "\tLine Offset: " << hex << (address_ & this->line_offset_mask) << endl;
            cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits) << endl;
            cout << "\tPage Offset: " << hex << (address_ & this->page_offset_mask) << endl;
            cout << "\tStatus: " << status << endl; 
            cout << "\tEvicted: " << evicted << endl;
            cout << "\tRead Value: " << static_cast<size_t>(*read_byte) << endl; 
            cout << "----------MAIN MEMORY'S CONTENTS----------" << endl;
            this->main_memory->print_memory_data();
            cout << "----------L1 CACHE'S CONTENTS----------" << endl;
            this->L1->print_cache_data();
            cout << "-----------------------------------------------------------" << endl;
        }

        return read_byte.value();
    }
}

void memory_subsystem::write(size_t address_, u_int8_t data_)
{
    string status;
    bool evicted = false;
    bool write_successful = this->L1->write_byte(address_, data_);
    if (write_successful)
    {
        status = "WRITE HIT";

        if (verbose)
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "\tTrace: WRITE " << address_ << " " << static_cast<size_t>(data_) << endl;
            cout << "\tTag: " << hex << (address_ >> (this->index_bits + this->line_offset_bits)) << endl;
            cout << "\tIndex: " << hex << ((address_ >> this->line_offset_bits) & this->index_mask) << endl;
            cout << "\tLine Offset: " << hex << (address_ & this->line_offset_mask) << endl;
            cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits) << endl;
            cout << "\tPage Offset: " << hex << (address_ & this->page_offset_mask) << endl;
            cout << "\tStatus: " << status << endl; 
            cout << "\tEvicted: " << evicted << endl;
            cout << "----------MAIN MEMORY'S CONTENTS----------" << endl;
            this->main_memory->print_memory_data();
            cout << "----------L1 CACHE'S CONTENTS----------" << endl;
            this->L1->print_cache_data();
            cout << "-----------------------------------------------------------" << endl;
        }
    }
    else
    {
        status = "WRITE MISS";

        // With no-write-allocate, we can just update the byte in memory without moving the line containing the byte to the cache
        if (!this->write_allocate)
        {
            this->main_memory->write_byte(data_, address_);
        }
        // With write allocate, we have to move the line containing the byte to the cache
        else
        {
            size_t offset_bits = log2(this->line_size);
            size_t offset_mask = (1u << offset_bits) - 1;
            size_t offset = address_ & offset_mask;
            memory_line* requested_line = this->main_memory->get_line(address_);
            vector<u_int8_t> requested_line_data = requested_line->get_line_data();
            requested_line_data[offset] = data_;
            optional<pair<vector<u_int8_t>, size_t>> evicted_line = this->L1->place_line(requested_line_data, address_);
            if (evicted_line.has_value())
            {
                vector<u_int8_t> evicted_line_data = evicted_line.value().first;
                this->main_memory->write_line(evicted_line_data, evicted_line.value().second);
                evicted = true;
            }
        }

        if (verbose)
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "\tTrace: WRITE " << address_ << " " << static_cast<size_t>(data_) << endl;
            cout << "\tTag: " << hex << (address_ >> (this->index_bits + this->line_offset_bits)) << endl;
            cout << "\tIndex: " << hex << ((address_ >> this->line_offset_bits) & this->index_mask) << endl;
            cout << "\tLine Offset: " << hex << (address_ & this->line_offset_mask) << endl;
            cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits) << endl;
            cout << "\tPage Offset: " << hex << (address_ & this->page_offset_mask) << endl;
            cout << "\tStatus: " << status << endl; 
            cout << "\tEvicted: " << evicted << endl;
            cout << "----------MAIN MEMORY'S CONTENTS----------" << endl;
            this->main_memory->print_memory_data();
            cout << "----------L1 CACHE'S CONTENTS----------" << endl;
            this->L1->print_cache_data();
            cout << "-----------------------------------------------------------" << endl;
        }
    }
}

void memory_subsystem::report_stats()
{
    cout << "--------------------CACHE STATISTICS--------------------" << endl;
    cout << "Total Accesses: " << dec << this->L1->total_accesses << endl;
    cout << "Total Hits: " << dec << this->L1->total_hits << endl;
    cout << "Read Hits: " << dec << this->L1->read_hits << endl;
    cout << "Write Hits: " << dec << this->L1->write_hits << endl;
    cout << "Total Misses: " << dec << this->L1->total_misses << endl;
    cout << "Read Misses: " << dec << this->L1->read_misses << endl;
    cout << "Write Misses: " << dec << this->L1->write_misses << endl;
    cout << "--------------------------------------------------------" << endl;
}