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
            this->fill_cache_line(source_line.value()->get_line_data(), address_, false, level);
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
            cout << "Status: " << status << " at cache " << this->cache_names[current_cache_level] << ", level " << current_cache_level << endl; 
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
            this->fill_cache_line(source_line_data, address_, false, level);
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
    bool cache_hit;
    // Check all levels of cache starting from level 0 till the data is found
    for (current_cache_level = 0; current_cache_level < this->num_cache_levels; current_cache_level++)
    {
        cache_hit = this->caches[current_cache_level]->find_byte(address_);
        if (cache_hit)
        {
            this->caches[current_cache_level]->update_write_hit_stats(address_);
            break;
        }
        else
            this->caches[current_cache_level]->update_write_miss_stats();
    }
    
    if (cache_hit)
    {
        status = "Hit";

        // Get the cache line from the level in which cache hit took place
        vector<u_int8_t> source_line_data = this->caches[current_cache_level]->get_cache_line(address_).value()->get_line_data();

        // Fill all cache levels from 0 till the level at which hit happened with the cache line
        bool update_memory = this->write_fill_path(source_line_data, current_cache_level, address_, data_, true);
        
        if (update_memory)
        {
            vector<u_int8_t> dirty_line_data = source_line_data;
            dirty_line_data[address_ & this->memory_line_offset_masks[current_memory_level]] = data_;
            this->memories[current_memory_level]->write_line(dirty_line_data, address_);
        }

        if (verbose)
        {
            cout << "Operation: WRITE " << address_ << " " << hex << static_cast<size_t>(data_) << endl;
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
            cout << "Status: " << status << " at cache " << this->cache_names[current_cache_level] << ", level " << current_cache_level << endl; 
            cout << "Evicted: " << evicted << endl;
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
    }
    else
    {
        status = "Miss";

        // Get the line's data from the memory
        vector<u_int8_t> source_line_data = this->memories[current_memory_level]->get_line(address_)->get_line_data();

        // Fill all cache levels
        bool update_memory = this->write_fill_path(source_line_data, current_cache_level, address_, data_, false);
        
        if (update_memory)
        {
            vector<u_int8_t> dirty_line_data = source_line_data;
            dirty_line_data[address_ & this->memory_line_offset_masks[current_memory_level]] = data_;
            this->memories[current_memory_level]->write_line(dirty_line_data, address_);
        }

        if (verbose)
        {
            cout << "Operation: WRITE " << address_ << " " << hex << static_cast<size_t>(data_) << endl;
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
    }
}

void memory_subsystem::fill_cache_line(vector<u_int8_t> line_data_, size_t address_, bool dirty_bit_, size_t cache_level_)
{
    optional<tuple<vector<u_int8_t>, size_t, bool>> evicted_line_info = this->caches[cache_level_]->insert_line(line_data_, address_, dirty_bit_); 
    
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

bool memory_subsystem::write_fill_path(vector<u_int8_t> source_line_data_, size_t cache_hit_level_, size_t address_, u_int8_t write_data_, bool hit_status_)
{
    bool all_lower_level_write_allocate = false;
    bool update_memory = true;
    string previous_level_write_policy, current_level_write_policy, hit_level_write_policy;
    bool current_level_write_allocate;
    vector<u_int8_t> dirty_line_data = source_line_data_;
    dirty_line_data[address_ & this->cache_line_offset_masks[0]] = write_data_;
    vector<u_int8_t> upper_level_line_data = source_line_data_;
    bool upper_level_dirty_bit = false;
    bool propagate_dirty_line_to_lower_level = true;

    // Write hit fill math
    if (hit_status_)
    {
        hit_level_write_policy = this->write_policies[cache_hit_level_];
        for (size_t level = 0; level < this->num_cache_levels; level++)
        {
            current_level_write_policy = this->write_policies[level];
            current_level_write_allocate = this->write_allocates[level];

            // Level 0 to cache_hit_leveL_ - 1; None of these level have the source cache line
            if (level < cache_hit_level_)
            {
                // Level 0
                if (level == 0)
                {
                    // Level 0's write allocate is true. If not, move to the next level
                    if (current_level_write_allocate)
                    {
                        all_lower_level_write_allocate = true;

                        dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                        upper_level_line_data = dirty_line_data;

                        // Since current level's policy is write back, we write the dirty cache line but with dirty bit as 1
                        if (current_level_write_policy == "write_back")
                        {
                            this->fill_cache_line(dirty_line_data, address_, true, level);
                            upper_level_dirty_bit = true;
                            propagate_dirty_line_to_lower_level = false;
                            update_memory = false;
                        }
                        // Since current level's policy is write through, we write the dirty cache line but with dirty bit as 0
                        else if (current_level_write_policy == "write_through")
                        {
                            this->fill_cache_line(dirty_line_data, address_, false, level);
                            upper_level_dirty_bit = false;
                        }
                    }
                }
                // Level 1 to cache_hit_level_ - 1
                else
                {
                    // All upper levels have allocated a line
                    if (all_lower_level_write_allocate)
                    {
                        // If previous level's write policy is write back, then that level has the dirty line with dirty bit as 1. Therefore, we put the source line with dirty bit as 0 in this level.
                        // If previous level's write policy is write through and that level has source line, it means some upper level cache must have been write back. Therefore, we put the source line with dirty bit as 0 in this level.
                        // if ((previous_level_write_policy == "write_back") || (previous_level_write_policy == "write_through" && upper_level_line_data == source_line_data_))
                        if ((previous_level_write_policy == "write_back") || (previous_level_write_policy == "write_through" && !propagate_dirty_line_to_lower_level))
                        {
                            this->fill_cache_line(source_line_data_, address_, false, level);
                            upper_level_line_data = source_line_data_;
                            upper_level_dirty_bit = false;
                        }
                        // Since previous level's policy is write through and current level's policy is write back, this level will have the dirty line with dirty bit as 1
                        else if (previous_level_write_policy == "write_through" && current_level_write_policy == "write_back")
                        {
                            this->fill_cache_line(dirty_line_data, address_, true, level);
                            upper_level_line_data = dirty_line_data;
                            upper_level_dirty_bit = true;
                            propagate_dirty_line_to_lower_level = false;
                            update_memory = false;
                        }
                        // Since previous level's policy is write through and current level's policy is write through too, this level will have the dirty line with dirty bit as 0
                        else if (previous_level_write_policy == "write_through" && current_level_write_policy == "write_through")
                        {
                            this->fill_cache_line(dirty_line_data, address_, false, level);
                            upper_level_line_data = dirty_line_data;
                            upper_level_dirty_bit = false;
                        }
                    }
                    // None of the upper levels have allocated a line so this is the first level where the line will be allocated
                    else if (current_level_write_allocate)
                    {
                        all_lower_level_write_allocate = true;

                        dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                        upper_level_line_data = dirty_line_data;

                        // Since policy is write back, we write the dirty cache line but with dirty bit as 1
                        if (current_level_write_policy == "write_back")
                        {
                            this->fill_cache_line(dirty_line_data, address_, true, level);
                            upper_level_dirty_bit = true;
                            propagate_dirty_line_to_lower_level = false;
                            update_memory = false;
                        }
                        // Since policy is write through, we write the dirty cache line but with dirty bit as 0
                        else if (current_level_write_policy == "write_through")
                        {
                            this->fill_cache_line(dirty_line_data, address_, false, level);
                            upper_level_dirty_bit = false;
                        }
                    }
                    // None of the upper levels as well as the current level have allocated a line so we move to the next level
                }
            }
            // Level cache_hit_level_ to num_cache_levels - 1; All these levels have the source cache line
            else
            {
                // One of the upper levels must have been write-allocate so it has the dirty cache line with dirty bit either 0 or 1
                if (all_lower_level_write_allocate)
                {
                    // Previous level has the dirty line with dirty bit as 0 due to its write through policy. Current level is write back so its dirty bit should be 1
                    if (previous_level_write_policy == "write_through" && current_level_write_policy == "write_back")
                    {
                        this->caches[level]->update(dirty_line_data, address_, true);
                        upper_level_line_data = dirty_line_data;
                        upper_level_dirty_bit = true;
                        propagate_dirty_line_to_lower_level = false;
                        update_memory = false;
                    }
                    // Previous level has the dirty line with dirty bit as 0 due its write through policu. Since current level is also write through, its dirty bit will be 0 as well.
                    else if (previous_level_write_policy == "write_through" && current_level_write_policy == "write_through")
                    {
                        this->caches[level]->update(dirty_line_data, address_, false);
                        upper_level_line_data = dirty_line_data;
                        upper_level_dirty_bit = false;
                    }
                    // Previous level was write back so it has the dirty line with dirty bit as 1. Therefore, current level will contain the source line and dirty bit as 0, which is already the case.
                    else
                    {
                        upper_level_line_data = source_line_data_;
                        upper_level_dirty_bit = false;
                    }
                }
                // This part of the code will be executed if level 0 to hit_cache_level_ - 1 are all no-write-allocate so hit_cache_level is the first level where dirty line is to be placed.
                else
                {
                    all_lower_level_write_allocate = true;

                    dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                    upper_level_line_data = dirty_line_data;

                    // Since current policy is write back, we write the dirty cache line but with dirty bit as 1
                    if (current_level_write_policy == "write_back")
                    {
                        this->caches[level]->update(dirty_line_data, address_, true);
                        upper_level_dirty_bit = true;
                        propagate_dirty_line_to_lower_level = false;
                        update_memory = false;
                    }
                    // Since current policy is write through, we write the dirty cache line but with dirty bit as 0
                    else if (current_level_write_policy == "write_through")
                    {
                        this->caches[level]->update(dirty_line_data, address_, false);
                        upper_level_dirty_bit = false;
                    }
                }
            }

            previous_level_write_policy = current_level_write_policy;
        }
    }
    // Write miss fill path
    else
    {
        for (size_t level = 0; level < this->num_cache_levels; level++)
        {
            current_level_write_policy = this->write_policies[level];
            current_level_write_allocate = this->write_allocates[level];

            if (level == 0)
            {
                if (current_level_write_allocate)
                {
                    all_lower_level_write_allocate = true;

                    dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                    upper_level_line_data = dirty_line_data;

                    if (current_level_write_policy == "write_back")
                    {
                        // Since policy is write back, we write the dirty cache line but with dirty bit as 1
                        this->fill_cache_line(dirty_line_data, address_, true, level);
                        upper_level_dirty_bit = true;
                        propagate_dirty_line_to_lower_level = false;
                        update_memory = false;
                    }
                    else if (current_level_write_policy == "write_through")
                    {
                        // Since policy is write through, we write the dirty cache line but with dirty bit as 0
                        this->fill_cache_line(dirty_line_data, address_, false, level);
                        upper_level_dirty_bit = false;
                    }
                }
            }
            else
            {
                // All upper levels have allocated a line
                if (all_lower_level_write_allocate)
                {
                    // if ((previous_level_write_policy == "write_back") || (previous_level_write_policy == "write_through" && upper_level_line_data == source_line_data_))
                    if ((previous_level_write_policy == "write_back") || (previous_level_write_policy == "write_through" && !propagate_dirty_line_to_lower_level))
                    {
                        this->fill_cache_line(source_line_data_, address_, false, level);
                        upper_level_line_data = source_line_data_;
                        upper_level_dirty_bit = false;
                    }
                    else if (previous_level_write_policy == "write_through" && current_level_write_policy == "write_back")
                    {
                        // dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                        this->fill_cache_line(dirty_line_data, address_, true, level);
                        upper_level_line_data = dirty_line_data;
                        upper_level_dirty_bit = true;
                        propagate_dirty_line_to_lower_level = false;
                        update_memory = false;

                    }
                    else if (previous_level_write_policy == "write_through" && current_level_write_policy == "write_through")
                    {
                        // dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                        this->fill_cache_line(upper_level_line_data, address_, false, level);
                        upper_level_line_data = dirty_line_data;
                        upper_level_dirty_bit = false;
                    }
                }
                // None of the upper levels have allocated a line so this is the first level where the line will be allocated
                else if (current_level_write_allocate)
                {
                    all_lower_level_write_allocate = true;

                    dirty_line_data[address_ & this->cache_line_offset_masks[level]] = write_data_;
                    upper_level_line_data = dirty_line_data;

                    if (current_level_write_policy == "write_back")
                    {
                        // Since policy is write back, we write the dirty cache line but with dirty bit as 1
                        this->fill_cache_line(dirty_line_data, address_, true, level);
                        upper_level_dirty_bit = true;
                        propagate_dirty_line_to_lower_level = false;
                        update_memory = false;
                    }
                    else if (current_level_write_policy == "write_through")
                    {
                        // Since policy is write through, we write the dirty cache line but with dirty bit as 0
                        this->fill_cache_line(dirty_line_data, address_, false, level);
                        upper_level_dirty_bit = false;
                    }
                }
                // None of the upper levels as well as the current level have allocated a line so we continue to the next level
            }
            previous_level_write_policy = current_level_write_policy;
        }
    }

    return update_memory;
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