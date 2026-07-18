#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <tuple>
#include <deque>
#include <algorithm>

#include "../../include/cache/cache.hpp"

using namespace std;

cache::cache(size_t cache_size_, size_t line_size_, size_t assoc_, string replacement_policy_)
{
    this->cache_size = cache_size_;
    this->line_size = line_size_;
    this->assoc = assoc_;
    this->replacement_policy = replacement_policy_;
    this->number_of_total_lines = this->cache_size/this->line_size;
    this->number_of_sets = this->number_of_total_lines / this->assoc;
    this->offset_bits = log2(this->line_size);
    this->offset_mask = (1u << this->offset_bits) - 1;
    this->index_bits = log2(this->cache_size/(this->line_size * this->assoc));
    this->index_mask = (1u << this->index_bits) - 1;
    this->gen = mt19937(0);

    if (this->cache_size % this->line_size != 0)
    {
        cout << "Cache size is not a multiple of line size" << endl;
    }

    for (size_t i = 0; i < this->number_of_total_lines; i++)
    {
        cache_line* l = new cache_line(this->line_size, false, "zeros");
        this->cache_lines.push_back(l);
        this->access_counts.push_back(0);
    }

    if (this->replacement_policy == "LRU")
    {
        for (size_t i = 0; i < this->number_of_sets; i++)
            this->access_order.push_back(deque<size_t>());
    }
    else if (this->replacement_policy == "PLRU")
    {
        for (size_t i = 0; i < this->number_of_sets; i++)
            this->pseudo_access_order.push_back(vector<bool>(this->assoc-1, false));
    }

    this->total_accesses = 0;
    this->read_accesses = 0;
    this->write_accesses = 0;

    this->read_hits = 0;
    this->read_misses = 0;
    this->write_hits = 0;
    this->write_misses = 0;

    this->total_hits = 0;
    this->total_misses = 0;
    this->total_evictions = 0;
    this->dirty_evictions = 0;
    this->non_dirty_eviction = 0;
    this->hit_rate = 0;
    this->miss_rate = 0;
}

std::optional<u_int8_t> cache::read_byte(size_t address_)
{
    size_t offset = address_ & this->offset_mask;
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    this->read_accesses += 1;
    this->total_accesses += 1;
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            u_int8_t byte = this->cache_lines[i]->get_byte(offset);
            this->access_counts[i] += 1;
            this->read_hits += 1;
            this->total_hits += 1;

            this->update_access_history(index, i);
            return byte;
        }
    }

    this->read_misses += 1;
    this->total_misses += 1;
    return nullopt;
}

bool cache::find_byte(size_t address_)
{
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            return true;
        }
    }

    return false;
}

void cache::write(vector<u_int8_t> line_data_, size_t address_, bool dirty_bit_)
{
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            this->cache_lines[i]->write_line(line_data_, tag, dirty_bit_);
            this->update_access_history(index, i);
        }
    }
}

void cache::update_evicted_line(vector<u_int8_t> line_data_, size_t address_, bool dirty_bit_)
{
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            this->cache_lines[i]->write_line(line_data_, tag, dirty_bit_);
        }
    }
}

optional<tuple<vector<u_int8_t>, size_t, bool>> cache::insert_line(vector<u_int8_t> line_data_, size_t address_, bool dirty_bit_)
{
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->index_bits + this->offset_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        // Check if there is a line in the set that is empty (i.e., valid bit is 0)
        if (!this->cache_lines[i]->get_valid())
        {
            this->cache_lines[i]->write_line(line_data_, tag, dirty_bit_);
            this->access_counts[i] = 0;
            this->update_access_history(index, i);
            return nullopt;
        }
    }

    this->total_evictions += 1;
    size_t line_number_to_replace = this->eviction_policy(index);
    vector<u_int8_t> evicted_line_data = this->cache_lines[line_number_to_replace]->get_line_data();
    bool evicted_line_dirty_bit = this->cache_lines[line_number_to_replace]->get_dirty_bit();
    size_t evicted_line_tag = this->cache_lines[line_number_to_replace]->get_tag();

    this->cache_lines[line_number_to_replace]->write_line(line_data_, tag, dirty_bit_);
    this->access_counts[line_number_to_replace] = 0;
    this->update_access_history(index, line_number_to_replace);


    // Return the evicted line if the dirty bit is 1, otherwise return nullptr
    if (evicted_line_dirty_bit)
    {
        this->dirty_evictions += 1;
    }
    else
    {
        this->non_dirty_eviction += 1;
    }   

    size_t evicted_line_address = (evicted_line_tag << (this->index_bits + this->offset_bits)) | (index << this->offset_bits);
    return make_tuple(evicted_line_data, evicted_line_address, evicted_line_dirty_bit);
}

size_t cache::eviction_policy(size_t index_)
{
    size_t line_number_to_evict;
    if (this->replacement_policy == "first_line")
    {
        line_number_to_evict = index_ * this->assoc;
        return line_number_to_evict;
    }
    else if (this->replacement_policy == "random")
    {
        uniform_int_distribution<size_t> dist(index_*this->assoc, (index_+1)*this->assoc - 1);
        line_number_to_evict = dist(this->gen);
        return line_number_to_evict;
    }
    else if (this->replacement_policy == "LFU")
    {
        int min_count = numeric_limits<int>::max();
        for (size_t i = index_*this->assoc; i < (index_+1)*this->assoc; i++)
        {
            if (this->access_counts[i] <= min_count)
            {
                min_count = this->access_counts[i];
                line_number_to_evict = i;
            }
        }
        return line_number_to_evict;
    }
    else if (this->replacement_policy == "MFU")
    {
        int max_count = numeric_limits<int>::min();
        for (size_t i = index_*this->assoc; i < (index_+1)*this->assoc; i++)
        {
            if (this->access_counts[i] >= max_count)
            {
                max_count = this->access_counts[i];
                line_number_to_evict = i;
            }
        }
        return line_number_to_evict; 
    }
    else if (this->replacement_policy == "LRU")
    {
        if (this->access_order[index_].size() != this->assoc)
        {
            cout << "Access order size is not equal to the associativity so no point in evicting a line. Please check the code." << endl;
            return -1;
        }
        else
        {
            return this->access_order[index_].back();
        }
    }
    else if (this->replacement_policy == "PLRU")
    {
        size_t node = 0;
        for (size_t i = 0; i < log2(this->assoc); i++)
        {
            if (!this->pseudo_access_order[index_][node])
                node = 2*node + 1;
            else
                node = 2*node + 2;
        }
        size_t way = node - (this->assoc - 1);
        size_t line_number = index_ * this->assoc + way;
        return line_number;
    }
    else
        throw invalid_argument("Unknown replacement policy");
}

optional<cache_line*> cache::get_cache_line(size_t address_)
{
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            return this->cache_lines[i];
        }
    }

    return nullopt;
}

void cache::update_access_history(size_t index_, size_t line_number_)
{
    if (this->replacement_policy == "LRU")
    {        
        // Deque is empty so just push the line_number_
        if (this->access_order[index_].size() == 0)
        {
            this->access_order[index_].push_front(line_number_);
        }
        // Size of deque is not equal to the associativity so still some empty space in the set 
        else if (this->access_order[index_].size() != this->assoc)
        {
            auto iter = find(this->access_order[index_].begin(), this->access_order[index_].end(), line_number_);
            if (iter == this->access_order[index_].end())
            {
                this->access_order[index_].push_front(line_number_);
            }
            else
            {
                this->access_order[index_].erase(iter);
                this->access_order[index_].push_front(line_number_);
            }
        }
        else
        {
            auto iter = find(this->access_order[index_].begin(), this->access_order[index_].end(), line_number_);
            if (iter == this->access_order[index_].end())
            {
                this->access_order[index_].pop_back();
                this->access_order[index_].push_front(line_number_);
            }
            else
            {
                this->access_order[index_].erase(iter);
                this->access_order[index_].push_front(line_number_);
            }
        }
    }
    else if (this->replacement_policy == "PLRU")
    {
        size_t way = (line_number_ - index_ * this->assoc);
        size_t child = way + (this->assoc - 1);
        size_t parent; 

        while (child > 0)
        {
            parent = (child - 1)/2;
            if (child == 2*parent + 1)
                this->pseudo_access_order[index_][parent] = true;
            else if (child == 2*parent + 2)
                this->pseudo_access_order[index_][parent] = false;
            child = parent;
        }
    }
}

vector<cache_line*> cache::get_cache_lines()
{
    return this->cache_lines;
}

vector<vector<u_int8_t>> cache::get_cache_data()
{
    vector<vector<u_int8_t>> cache_data;
    
    for (size_t i = 0; i < this->number_of_total_lines; i++)
    {
        cache_data.push_back(this->cache_lines[i]->get_line_data());
    }

    return cache_data;
}

void cache::update_write_hit_stats(size_t address_)
{
    this->total_accesses += 1;
    this->write_accesses += 1;
    
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            this->access_counts[i] += 1;
            this->write_hits += 1;
            this->total_hits += 1;
        }
    }
}

void cache::update_write_miss_stats()
{
    this->total_accesses += 1;
    this->write_accesses += 1;

    this->write_misses += 1;
    this->total_misses += 1;
}

void cache::print_cache_data()
{
    cout << "Line #\tValid\tDirty\tTag\tData" << endl;
    for (size_t i = 0; i < this->number_of_total_lines; i++)
    {
        cout << i << "\t";
        this->cache_lines[i]->print_line_data();
        cout << endl;
    }
}