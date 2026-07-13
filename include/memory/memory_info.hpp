#pragma once

#include <string>
#include <cstddef>

struct MemoryInfo
{
    std::string name;
    std::size_t memory_size;
    std::size_t page_size;
    std::size_t line_size;
};
