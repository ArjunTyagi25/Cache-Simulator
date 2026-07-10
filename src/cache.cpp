#include<iostream>
#include<optional>
#include<random>
#include<stdexcept>
#include<cache.hpp>

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
        line* l = new line(this->line_size, false, "zeros");
        this->cache_lines.push_back(l);
        this->access_counts.push_back(0);
    }
}

std::optional<u_int8_t> cache::find_byte(size_t address_)
{
    size_t offset = address_ & this->offset_mask;
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            u_int8_t byte = this->cache_lines[i]->get_byte(offset);
            this->access_counts[i] += 1;
            return byte;
        }
    }

    return nullopt;
}

bool cache::write_byte(size_t address_, u_int8_t write_data_)
{
    size_t offset = address_ & this->offset_mask;
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->offset_bits + this->index_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        // Check if line is already present in the cache
        if (this->cache_lines[i]->get_tag() == tag && this->cache_lines[i]->get_valid())
        {
            this->cache_lines[i]->write_byte(write_data_, tag, offset);
            this->access_counts[i] += 1;
            return true;
        }
    }

    // The line which contains the address where we wanna write is not present in the cache. 
    // We return 0 to indicate that write operation has not completed succesfully.
    // Next step would be to get the line containing that byte address from a lower-level cache and write that entire line into the higher-level cache.
    return false;
}

// This function places a new line in the cache by either finding an empty spot or evicting an line.
pair<line*, size_t> cache::replace_line(line* new_line_, size_t address_)
{
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    size_t tag = (address_ >> (this->index_bits + this->offset_bits));
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        // Check if there is a line in the set that is empty (i.e., valid bit is 0)
        if (!this->cache_lines[i]->get_valid())
        {
            this->cache_lines[i] = new_line_;
            this->access_counts[i] = 0;
            this->cache_lines[i]->set_tag(tag);
            return make_pair(nullptr, NULL);
        }
    }

    size_t line_number_to_replace = this->eviction_policy(index);
    line* evicted_line = this->cache_lines[line_number_to_replace];
    this->cache_lines[line_number_to_replace] = new_line_;
    this->access_counts[line_number_to_replace] = 0;
    this->cache_lines[line_number_to_replace]->set_tag(tag);

    // Return the evicted line if the dirty bit is 1, otherwise return nullptr
    if (evicted_line->get_dirty_bit())
    {
        size_t evicted_line_address = (evicted_line->get_tag().value() << (this->index_bits + this->offset_bits)) | (index << this->offset_bits);
        return make_pair(evicted_line, evicted_line_address);
    }
    else   
        return make_pair(nullptr, NULL);
}

size_t cache::eviction_policy(size_t index)
{
    size_t line_number_to_evict;
    if (this->replacement_policy == "first_line")
    {
        line_number_to_evict = index * this->assoc;
        return line_number_to_evict;
    }
    else if (this->replacement_policy == "random")
    {
        uniform_int_distribution<size_t> dist(index*this->assoc, (index+1)*this->assoc - 1);
        line_number_to_evict = dist(this->gen);
        return line_number_to_evict;
    }
    else if (this->replacement_policy == "LFU")
    {
        int min_count = numeric_limits<int>::max();
        for (size_t i = index*this->assoc; i < (index+1)*this->assoc; i++)
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
        for (size_t i = index*this->assoc; i < (index+1)*this->assoc; i++)
        {
            if (this->access_counts[i] >= max_count)
            {
                max_count = this->access_counts[i];
                line_number_to_evict = i;
            }
        }
        return line_number_to_evict; 
    }
    else
        throw invalid_argument("Unknown replacement policy");
}

vector<line*> cache::get_cache_lines()
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

void cache::print_cache_data()
{
    cout << "Line #\tValid\tDirty\tTag\tData" << endl;
    for (size_t i = 0; i < this->number_of_total_lines; i++)
    {
        cout << i << "\t";
        this->cache_lines[i]->print_line_data(true);
        cout << endl;
    }
}