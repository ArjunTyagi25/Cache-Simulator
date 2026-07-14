#include <string>
#include <iostream>
#include <optional>

#include "../include/memory_subsystem.hpp"

using namespace std;

memory_subsystem::memory_subsystem(size_t num_memory_levels_,
                                   vector<MemoryInfo> memory_infos_,
                                   size_t num_cache_levels_,
                                   vector<CacheInfo> cache_infos_,
                                   bool verbose_)
{
    this->verbose = verbose_;

    this->num_memory_levels = num_memory_levels_;
    for (size_t level = 0; level < this->num_memory_levels; level++)
    {
        this->memory_names.push_back(memory_infos_[level].name);
        this->memory_sizes.push_back(memory_infos_[level].memory_size);
        this->page_sizes.push_back(memory_infos_[level].page_size);
        this->memory_line_sizes.push_back(memory_infos_[level].line_size);

        this->memory_line_offset_bits.push_back(log2(this->memory_line_sizes[level]));
        this->memory_line_offset_masks.push_back((1u << this->memory_line_offset_bits[level]) - 1);

        this->page_offset_bits.push_back(log2(this->page_sizes[level]));
        this->page_offset_masks.push_back((1u << this->page_offset_bits[level]) - 1);

        memory* m = new memory(this->memory_sizes[level], this->page_sizes[level], this->memory_line_sizes[level], "random");
        this->memories.push_back(m);

        if (this->verbose)
        {
            cout << "--------------------INITIAL CONTENT OF " << this->memory_names[level] << " MEMORY--------------------" << endl;
            this->memories[level]->print_memory_data();
        }
    }
    
    this->num_cache_levels = num_cache_levels_;
    for (size_t level = 0; level < this->num_cache_levels; level++)
    {
        this->cache_names.push_back(cache_infos_[level].name);
        this->cache_sizes.push_back(cache_infos_[level].cache_size);
        this->cache_line_sizes.push_back(cache_infos_[level].line_size);
        this->assocs.push_back(cache_infos_[level].assoc);
        this->replacement_policies.push_back(cache_infos_[level].replacement_policy);
        this->write_policies.push_back(cache_infos_[level].write_policy);
        this->write_allocates.push_back(cache_infos_[level].write_allocate);

        this->cache_line_offset_bits.push_back(log2(this->cache_line_sizes[level]));
        this->cache_line_offset_masks.push_back((1u << this->cache_line_offset_bits[level]) - 1);

        this->index_bits.push_back(log2(this->cache_sizes[level]/(this->cache_line_sizes[level] * this->assocs[level])));
        this->index_masks.push_back((1u << this->index_bits[level]) - 1);
        
        cache* c = new cache(this->cache_sizes[level], this->cache_line_sizes[level], this->assocs[level], this->replacement_policies[level]);
        this->caches.push_back(c);

        if (this->verbose)
        {
            cout << "--------------------INITIAL CONTENT OF " << this->cache_names[level] << " CACHE---------------------" << endl;
            this->caches[level]->print_cache_data();
        }
    }

    if (this->verbose)
        cout << "-------------------------------------------------------------------------------------------" << endl;
}

u_int8_t memory_subsystem::read(size_t address_)
{
    size_t current_cache_level;
    size_t current_memory_level = 0;

    string status;
    bool evicted = false;
    optional<u_int8_t> read_byte;

    // Check all levels of cache starting from level 0 till the data is found
    for (current_cache_level = 0; current_cache_level < this->num_cache_levels; current_cache_level++)
    {
        read_byte = this->caches[current_cache_level]->read_byte(address_);
        if (read_byte.has_value())
            break;
    }

    // If current_cache_level != num_cache_level, that means the address was found in one of the caches
    if (current_cache_level != this->num_cache_levels)
    {
        optional<cache_line*> source_line = this->caches[current_cache_level]->get_cache_line(address_);
        // Starting from highest level (0) going downwards, we need to put the line containing address_ in all higher-level caches
        for (size_t level = 0; level < current_cache_level; level++)
        {
            this->fill_cache_line(source_line.value()->get_line_data(), address_, level);
        }
    }

    if (read_byte.has_value())
    {
        status = "Hit";

        if (verbose)
        {
            cout << "Operation: READ " << address_ << endl;
            for (size_t level = 0; level < this->num_cache_levels; level++)
            {
                cout << this->cache_names[level] << "'s TAG, INDEX, OFFSET" << endl; 
                cout << "\tTag: " << hex << (address_ >> (this->index_bits[level] + this->cache_line_offset_bits[level])) << endl;
                cout << "\tIndex: " << hex << ((address_ >> this->cache_line_offset_bits[level]) & this->index_masks[level]) << endl;
                cout << "\tLine Offset: " << hex << (address_ & this->cache_line_offset_masks[level]) << endl;
            }
            for (size_t level = 0; level < this->num_memory_levels; level++)
            {
                cout << this->memory_names[level] << "'s PPN & OFFSET" << endl;
                cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits[level]) << endl;
                cout << "\tPage Offset: " << hex << (address_ & this->page_offset_masks[level]) << endl;
            }
            cout << "\tStatus: " << status << " at cache " << this->cache_names[current_cache_level] << ", level " << current_cache_level << endl; 
            cout << "Evicted: " << evicted << endl;
            cout << "Read Value: " << static_cast<size_t>(*read_byte) << endl; 
            for (size_t level = 0; level < this->num_memory_levels; level++)
            {
                cout << "--------------------CONTENT OF " << this->memory_names[level] << " MEMORY--------------------" << endl;
                this->memories[level]->print_memory_data();
            }
            for (size_t level = 0; level < this->num_cache_levels; level++)
            {
                cout << "--------------------CONTENT OF " << this->cache_names[level] << " CACHE---------------------" << endl;
                this->caches[level]->print_cache_data();
            }
            cout << "-----------------------------------------------------------" << endl;
        }

        return read_byte.value();
    }
    else
    {
        status = "Miss";

        // Fetch the line from the memory
        memory_line* source_line = this->memories[current_memory_level]->get_line(address_);
        vector<u_int8_t> source_line_data = source_line->get_line_data();

        size_t offset = address_ & this->memory_line_offset_masks[current_memory_level];
        read_byte = source_line_data[offset];
        
        // Start from level 0 till num_cache_level inserting the cache line containing address_
        for (size_t level = 0; level < current_cache_level; level++)
        {
            this->fill_cache_line(source_line_data, address_, level);
        }

        if (verbose)
        {
            cout << "Operation: READ " << address_ << endl;
            for (size_t level = 0; level < this->num_cache_levels; level++)
            {
                cout << this->cache_names[level] << "'s TAG, INDEX, OFFSET" << endl; 
                cout << "\tTag: " << hex << (address_ >> (this->index_bits[level] + this->cache_line_offset_bits[level])) << endl;
                cout << "\tIndex: " << hex << ((address_ >> this->cache_line_offset_bits[level]) & this->index_masks[level]) << endl;
                cout << "\tLine Offset: " << hex << (address_ & this->cache_line_offset_masks[level]) << endl;
            }
            for (size_t level = 0; level < this->num_memory_levels; level++)
            {
                cout << this->memory_names[level] << "'s PPN & OFFSET" << endl;
                cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits[level]) << endl;
                cout << "\tPage Offset: " << hex << (address_ & this->page_offset_masks[level]) << endl;
            }
            cout << "Status: " << status << " at all cache levels." << endl; 
            cout << "Evicted: " << evicted << endl;
            cout << "Read Value: " << static_cast<size_t>(*read_byte) << endl; 
            for (size_t level = 0; level < this->num_memory_levels; level++)
            {
                cout << "--------------------CONTENT OF " << this->memory_names[level] << " MEMORY--------------------" << endl;
                this->memories[level]->print_memory_data();
            }
            for (size_t level = 0; level < this->num_cache_levels; level++)
            {
                cout << "--------------------CONTENT OF " << this->cache_names[level] << " CACHE---------------------" << endl;
                this->caches[level]->print_cache_data();
            }
            cout << "-----------------------------------------------------------" << endl;
        }

        return read_byte.value();
    }
}

void memory_subsystem::write(size_t address_, u_int8_t data_)
{
    size_t current_cache_level = 0;
    size_t current_memory_level = 0;

    string status;
    bool evicted = false;
    bool write_successful = this->caches[current_cache_level]->write_byte(address_, data_);
    if (write_successful)
    {
        status = "WRITE HIT";

        // Since the write policy is write-through, the same byte has to be written to the main memory too
        if (this->write_policies[current_cache_level] == "write_through")
            this->memories[current_memory_level]->write_byte(data_, address_);

        if (verbose)
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "\tTrace: WRITE " << address_ << " " << static_cast<size_t>(data_) << endl;
            cout << "\tTag: " << hex << (address_ >> (this->index_bits[current_cache_level] + this->cache_line_offset_bits[current_cache_level])) << endl;
            cout << "\tIndex: " << hex << ((address_ >> this->cache_line_offset_bits[current_cache_level]) & this->index_masks[current_cache_level]) << endl;
            cout << "\tLine Offset: " << hex << (address_ & this->cache_line_offset_masks[current_cache_level]) << endl;
            cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits[current_cache_level]) << endl;
            cout << "\tPage Offset: " << hex << (address_ & this->page_offset_masks[current_cache_level]) << endl;
            cout << "\tStatus: " << status << endl; 
            cout << "\tEvicted: " << evicted << endl;
            cout << "----------MAIN MEMORY'S CONTENTS----------" << endl;
            this->memories[current_memory_level]->print_memory_data();
            cout << "----------L1 CACHE'S CONTENTS----------" << endl;
            this->caches[current_cache_level]->print_cache_data();
            cout << "-----------------------------------------------------------" << endl;
        }
    }
    else
    {
        status = "WRITE MISS";

        // With no-write-allocate, we can just update the byte in memory without moving the line containing the byte to the cache
        if (!this->write_allocates[current_cache_level])
        {
            this->memories[current_memory_level]->write_byte(data_, address_);
        }
        // With write allocate, we have to move the line containing the byte to the cache
        else
        {
            size_t offset = address_ & this->cache_line_offset_masks[current_cache_level];
            memory_line* requested_line = this->memories[current_memory_level]->get_line(address_);
            vector<u_int8_t> requested_line_data = requested_line->get_line_data();
            requested_line_data[offset] = data_;
            optional<tuple<vector<u_int8_t>, size_t, bool>> evicted_line;
            
            if (this->write_policies[current_cache_level] == "write_back")
                evicted_line = this->caches[current_cache_level]->insert_line(requested_line_data, address_, true);
            else if (this->write_policies[current_cache_level] == "write_through")
                evicted_line = this->caches[current_cache_level]->insert_line(requested_line_data, address_, false);
                 
            if (evicted_line.has_value())
            {
                vector<u_int8_t> evicted_line_data = get<0>(evicted_line.value());
                this->memories[current_memory_level]->write_line(evicted_line_data, get<1>(evicted_line.value()));
                evicted = true;
            }

            // If the write policy is write-through, update the memory as well
            if (this->write_policies[current_cache_level] == "write_through")
                this->memories[current_memory_level]->write_byte(data_, address_);
        }

        if (verbose)
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "\tTrace: WRITE " << address_ << " " << static_cast<size_t>(data_) << endl;
            cout << "\tTag: " << hex << (address_ >> (this->index_bits[current_cache_level] + this->cache_line_offset_bits[current_cache_level])) << endl;
            cout << "\tIndex: " << hex << ((address_ >> this->cache_line_offset_bits[current_cache_level]) & this->index_masks[current_cache_level]) << endl;
            cout << "\tLine Offset: " << hex << (address_ & this->cache_line_offset_masks[current_cache_level]) << endl;
            cout << "\tPage Number: " << hex << (address_ >> this->page_offset_bits[current_cache_level]) << endl;
            cout << "\tPage Offset: " << hex << (address_ & this->page_offset_masks[current_cache_level]) << endl;
            cout << "\tStatus: " << status << endl; 
            cout << "\tEvicted: " << evicted << endl;
            cout << "----------MAIN MEMORY'S CONTENTS----------" << endl;
            this->memories[current_memory_level]->print_memory_data();
            cout << "----------L1 CACHE'S CONTENTS----------" << endl;
            this->caches[current_cache_level]->print_cache_data();
            cout << "-----------------------------------------------------------" << endl;
        }
    }
}

void memory_subsystem::fill_cache_line(vector<u_int8_t> line_data_, size_t address_, size_t cache_level_)
{
    optional<tuple<vector<u_int8_t>, size_t, bool>> evicted_line_info = this->caches[cache_level_]->insert_line(line_data_, address_, false); 
    
    // A valid line was evicted from level cache_level_
    if (evicted_line_info.has_value())
    {
        // If cache_level_ is 0, we may need to write the evicted line to the next lower-level cache
        if (cache_level_ == 0)
        {
            // Dirty bit is 1 so need to write the evicted line to level 1 cache
            if (get<2>(evicted_line_info.value()))
            {
                // Update the evicted line in lower level cache, if it exists
                if (cache_level_+1 != this->num_cache_levels)
                    this->caches[cache_level_+1]->update(get<0>(evicted_line_info.value()), get<1>(evicted_line_info.value()), get<2>(evicted_line_info.value()));
                else
                    this->memories[0]->write_line(get<0>(evicted_line_info.value()), get<1>(evicted_line_info.value()));
            }
            // Dirty bit is 0 so no need to update lower-level cache
        }
        // If cache_level_ is not 0
        else
        {
            // Invalidating evicted line in upper-level caches
            this->invalidate_upper_level_copies(get<1>(evicted_line_info.value()), cache_level_);

            // Dirty bit is 1 so need to write the evicted line to next lower-level cache
            if (get<2>(evicted_line_info.value()))
            {
                // Update the evicted line in lower-level cache, if it exist
                if (cache_level_+1 != this->num_cache_levels)
                    this->caches[cache_level_+1]->update(get<0>(evicted_line_info.value()), get<1>(evicted_line_info.value()), get<2>(evicted_line_info.value()));
                else
                    this->memories[0]->write_line(get<0>(evicted_line_info.value()), get<1>(evicted_line_info.value()));
            }
            // Dirty bit is 0 so no need to update lower-level cache
        }
    }
}

void memory_subsystem::invalidate_upper_level_copies(size_t address_, size_t level_)
{
    for (size_t level = 0; level < level_; level++)
    {
        optional<cache_line*> upper_level_line = this->caches[level]->get_cache_line(address_);
        if (upper_level_line.has_value())
        {
            if (upper_level_line.value()->get_dirty_bit())
                this->caches[level_+1]->update(upper_level_line.value()->get_line_data(), address_, true);
            upper_level_line.value()->set_valid(false);
        }
    }
}

void memory_subsystem::report_stats()
{
    cout << "--------------------CACHE STATISTICS--------------------" << endl;
    for (size_t level = 0; level < this->num_cache_levels; level++)
    {
        cout << "----------------" << this->cache_names[level] << " STATISTICS----------------" << endl;
        cout << "\tTotal Accesses: " << dec << this->caches[level]->total_accesses << endl;
        cout << "\tTotal Hits: " << dec << this->caches[level]->total_hits << endl;
        cout << "\t\tRead Hits: " << dec << this->caches[level]->read_hits << endl;
        cout << "\t\tWrite Hits: " << dec << this->caches[level]->write_hits << endl;
        cout << "\tTotal Misses: " << dec << this->caches[level]->total_misses << endl;
        cout << "\t\tRead Misses: " << dec << this->caches[level]->read_misses << endl;
        cout << "\t\tWrite Misses: " << dec << this->caches[level]->write_misses << endl;
        cout << "\tTotal Evictions: " << dec << this->caches[level]->total_evictions << endl;
        cout << "\t\tDirty Evictions: " << dec << this->caches[level]->dirty_evictions << endl;
        cout << "\t\tNon-Dirty Evictions: " << dec << this->caches[level]->non_dirty_eviction << endl;
    }
}