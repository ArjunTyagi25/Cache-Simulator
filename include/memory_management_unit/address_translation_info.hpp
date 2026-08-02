#pragma once

#include <string>
#include <cstddef>

struct AddressTranslationInfo
{
    std::size_t num_bits_virtual_address;
    std::size_t num_bits_physical_address;
    std::size_t num_levels_page_table;
    std::size_t PTE_size;
    std::size_t TLB_size;
    std::string allocation_policy;
};