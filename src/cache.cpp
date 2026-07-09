#include<iostream>
#include<optional>
#include<cache.hpp>

using namespace std;

cache::cache(size_t cache_size_, size_t line_size_, size_t assoc_)
{
    this->cache_size = cache_size_;
    this->line_size = line_size_;
    this->assoc = assoc_;
    this->number_of_total_lines = this->cache_size/this->line_size;
    this->number_of_sets = this->number_of_total_lines / this->assoc;
    this->offset_bits = log2(this->line_size);
    this->offset_mask = (1u << this->offset_bits) - 1;
    this->index_bits = log2(this->cache_size/(this->line_size * this->assoc));
    this->index_mask = (1u << this->index_bits) - 1;

    if (this->cache_size % this->line_size != 0)
    {
        cout << "Cache size is not a multiple of line size" << endl;
    }

    for (size_t i = 0; i < this->number_of_total_lines; i++)
    {
        line* l = new line(this->line_size, false, "zeros");
        this->cache_lines.push_back(l);
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
            return true;
        }
    }

    // The line which contains the address where we wanna write is not present in the cache. 
    // We return 0 to indicate that write operation has not completed succesfully.
    // Next step would be to get the line containing that byte address from a lower-level cache and write that entire line into the higher-level cache.
    return false;
}

pair<line*, size_t> cache::replace_line(line* new_line_, size_t address_)
{
    // This function places a new line in the cache by either finding an empty spot or evicting an line.
    size_t index = (address_ >> this->offset_bits) & this->index_mask;
    for (size_t i = index*this->assoc; i < (index+1) * this->assoc; i++)
    {
        // Check if there is a line in the set that is empty (i.e., valid bit is 0)
        if (!this->cache_lines[i]->get_valid())
        {
            this->cache_lines[i] = new_line_;
            return make_pair(nullptr, NULL);
        }
    }

    // None of the line in the set is empty, so we evict one line from the set to make space for the new line
    // TO-DO: implement eviction policy. 
    line* evicted_line = this->cache_lines[index * this->assoc];
    this->cache_lines[index * this->assoc] = new_line_;

    // Return the evicted line if the dirty bit is 1, otherwise return nullptr
    if (evicted_line->get_dirty_bit())
    {
        size_t evicted_line_address = (evicted_line->get_tag() << (this->index_bits + this->offset_bits)) | (index << this->offset_bits);
        return make_pair(evicted_line, evicted_line_address);
    }
    else   
        return make_pair(nullptr, NULL);

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
        this->cache_lines[i]->print_line_data();
        cout << endl;
    }
}