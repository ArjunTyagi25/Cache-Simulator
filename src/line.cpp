#include<line.hpp>

line::line(size_t line_size_)
{
    this->line_size = line_size_;
    for (size_t i = 0; i < this->line_size; i++)
    {
        this->line_data.push_back(0x00);
    }
    this->valid = false;
}

u_int8_t line::get_byte(size_t offset_)
{
    return this->line_data[offset_];
}

vector<u_int8_t> line::get_line_data()
{
    return this->line_data;
}

void line::write_byte(u_int8_t write_data_, size_t tag_, size_t offset_)
{
    this->line_data[offset_] = write_data_;
    this->tag = tag_;
    this->valid = true;
}

void line::write_line(vector<u_int8_t> write_data_, size_t tag_)
{
    this->line_data = write_data_;
    this->tag = tag_;
    this->valid = true;
}

size_t line::get_tag()
{
    return this->tag;
}

bool line::get_valid()
{
    return this->valid;
}