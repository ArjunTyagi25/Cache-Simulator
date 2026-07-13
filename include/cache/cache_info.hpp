#pragma once

#include <string>
#include <cstddef>

struct CacheInfo
{
    std::string name;
    std::size_t cache_size;
    std::size_t line_size;
    std::size_t assoc;
    std::string replacement_policy;
    std::string write_policy;
    bool write_allocate;
};
